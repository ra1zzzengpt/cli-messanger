from __future__ import annotations

from flask import Flask, jsonify, request
from datetime import datetime
from pathlib import Path
from threading import Lock
import json
import time
import os

import bcrypt

app = Flask(__name__)
start_time = time.time()

STATE_FILE = Path("server_state.json")
state_lock = Lock()

DEBUG_MODE = os.environ.get("DEBUG_MODE", "0") == "1"


def default_state() -> dict:
    return {
        "next_message_id": 1,
        "users": {},
        "messages": []
    }


def load_state() -> dict:
    if not STATE_FILE.exists():
        return default_state()

    try:
        with STATE_FILE.open("r", encoding="utf-8") as file:
            data = json.load(file)

        if not isinstance(data, dict):
            return default_state()

        data.setdefault("next_message_id", 1)
        data.setdefault("users", {})
        data.setdefault("messages", [])

        return data

    except json.JSONDecodeError:
        return default_state()


def save_state(state: dict) -> None:
    with STATE_FILE.open("w", encoding="utf-8") as file:
        json.dump(state, file, ensure_ascii=False, indent=4)


state = load_state()


def ok_response(data: dict | None = None, status_code: int = 200):
    body = {"ok": True}
    if data:
        body.update(data)
    return jsonify(body), status_code


def error_response(message: str, status_code: int = 400):
    return jsonify({"ok": False, "error": message}), status_code


def get_json_body() -> dict | None:
    data = request.get_json(silent=True)
    if not isinstance(data, dict):
        return None
    return data


def parse_uint64(value) -> int | None:
    try:
        number = int(value)
    except (TypeError, ValueError):
        return None
    if number <= 0:
        return None
    return number


def now_timestamp() -> str:
    return datetime.now().strftime("%m.%d %H:%M:%S")


def get_user_by_id_unlocked(user_id: int) -> dict | None:
    return state["users"].get(str(user_id))


def hash_password(password: str) -> str:
    return bcrypt.hashpw(password.encode("utf-8"), bcrypt.gensalt()).decode("utf-8")


def verify_password_unlocked(user: dict, password: str) -> bool:
    stored = user.get("password", "")
    if not stored:
        return False
    return bcrypt.checkpw(password.encode("utf-8"), stored.encode("utf-8"))


def public_user(user: dict) -> dict:
    return {
        "id": str(user["id"]),
        "nick": user["nick"]
    }


def public_message(message: dict) -> dict:
    from_user = get_user_by_id_unlocked(message["from_id"])
    from_nick = from_user["nick"] if from_user else "unknown"
    return {
        "id": str(message["id"]),
        "from_id": str(message["from_id"]),
        "from_nick": from_nick,
        "to_id": str(message["to_id"]),
        "text": message["text"],
        "created_at": message["created_at"]
    }


@app.get("/ping")
def ping():
    uptime_seconds = int(time.time() - start_time)
    return ok_response({
        "status": "online",
        "uptime": f"{uptime_seconds}s"
    })


@app.get("/health")
def health():
    return ok_response({"service": "cli-messenger-dev-server"})


@app.post("/users/register")
def register_user():
    data = get_json_body()
    if data is None:
        return error_response("expected JSON object", 400)

    user_id = parse_uint64(data.get("id"))
    nick = data.get("nick")
    password = data.get("password")

    if user_id is None:
        return error_response("invalid user id", 400)

    if not isinstance(nick, str) or not nick.strip():
        return error_response("invalid nick", 400)

    if not isinstance(password, str) or len(password) < 4:
        return error_response("password must be at least 4 characters", 400)

    nick = nick.strip()

    if len(nick) > 32:
        return error_response("nick is too long", 400)

    with state_lock:
        if get_user_by_id_unlocked(user_id) is not None:
            return error_response("user id already exists", 409)

        user = {
            "id": user_id,
            "nick": nick,
            "password": hash_password(password),
            "created_at": now_timestamp()
        }

        state["users"][str(user_id)] = user
        save_state(state)

    return ok_response({"user": public_user(user)}, 201)


@app.post("/users/login")
def login_user():
    data = get_json_body()
    if data is None:
        return error_response("expected JSON object", 400)

    user_id = parse_uint64(data.get("id"))
    password = data.get("password")

    if user_id is None:
        return error_response("invalid user id", 400)

    if not isinstance(password, str):
        return error_response("invalid password", 400)

    with state_lock:
        user = get_user_by_id_unlocked(user_id)

        if user is None:
            return error_response("user not found", 404)

        if not verify_password_unlocked(user, password):
            return error_response("invalid password", 401)

        return ok_response({"user": public_user(user)})


@app.post("/users/get")
def get_user():
    data = get_json_body()
    if data is None:
        return error_response("expected JSON object", 400)

    user_id = parse_uint64(data.get("id"))
    if user_id is None:
        return error_response("invalid user id", 400)

    password = data.get("password")
    if not isinstance(password, str):
        return error_response("password required", 401)

    with state_lock:
        user = get_user_by_id_unlocked(user_id)
        if user is None:
            return error_response("user not found", 404)
        if not verify_password_unlocked(user, password):
            return error_response("invalid password", 401)
        return ok_response({"user": public_user(user)})


@app.patch("/users/<user_id>/nick")
def update_nick(user_id: str):
    parsed_user_id = parse_uint64(user_id)
    if parsed_user_id is None:
        return error_response("invalid user id", 400)

    data = get_json_body()
    if data is None:
        return error_response("expected JSON object", 400)

    password = data.get("password")
    new_nick = data.get("nick")

    if not isinstance(password, str):
        return error_response("password required", 401)

    if not isinstance(new_nick, str) or not new_nick.strip():
        return error_response("invalid nick", 400)

    new_nick = new_nick.strip()

    if len(new_nick) > 32:
        return error_response("nick is too long", 400)

    with state_lock:
        user = get_user_by_id_unlocked(parsed_user_id)
        if user is None:
            return error_response("user not found", 404)

        if not verify_password_unlocked(user, password):
            return error_response("invalid password", 401)

        user["nick"] = new_nick
        save_state(state)

        return ok_response({"user": public_user(user)})


@app.patch("/users/<user_id>/password")
def update_password(user_id: str):
    parsed_user_id = parse_uint64(user_id)
    if parsed_user_id is None:
        return error_response("invalid user id", 400)

    data = get_json_body()
    if data is None:
        return error_response("expected JSON object", 400)

    old_password = data.get("old_password")
    new_password = data.get("password")

    if not isinstance(old_password, str):
        return error_response("old_password required", 400)

    if not isinstance(new_password, str) or len(new_password) < 4:
        return error_response("password must be at least 4 characters", 400)

    with state_lock:
        user = get_user_by_id_unlocked(parsed_user_id)
        if user is None:
            return error_response("user not found", 404)

        if not verify_password_unlocked(user, old_password):
            return error_response("invalid current password", 401)

        user["password"] = hash_password(new_password)
        save_state(state)

        return ok_response({"user": public_user(user)})


@app.post("/messages/send")
def send_message():
    data = get_json_body()
    if data is None:
        return error_response("expected JSON object", 400)

    from_id = parse_uint64(data.get("from_id"))
    to_id = parse_uint64(data.get("to_id"))
    password = data.get("password")
    text = data.get("text")

    if from_id is None:
        return error_response("invalid from_id", 400)

    if to_id is None:
        return error_response("invalid to_id", 400)

    if from_id == to_id:
        return error_response("cannot send message to yourself", 400)

    if not isinstance(password, str):
        return error_response("password required", 401)

    if not isinstance(text, str) or not text.strip():
        return error_response("message text is empty", 400)

    text = text.strip()

    if len(text) > 4000:
        return error_response("message text is too long", 400)

    with state_lock:
        sender = get_user_by_id_unlocked(from_id)
        receiver = get_user_by_id_unlocked(to_id)

        if sender is None:
            return error_response("sender not found", 404)

        if not verify_password_unlocked(sender, password):
            return error_response("invalid password", 401)

        if receiver is None:
            return error_response("receiver not found", 404)

        message = {
            "id": state["next_message_id"],
            "from_id": from_id,
            "to_id": to_id,
            "text": text,
            "created_at": now_timestamp()
        }

        state["next_message_id"] += 1
        state["messages"].append(message)
        save_state(state)

        return ok_response({
            "message_id": message["id"],
            "message": public_message(message)
        }, 201)


@app.post("/messages/dump")
def dump_messages():
    data = get_json_body()
    if data is None:
        return error_response("expected JSON object", 400)

    me = parse_uint64(data.get("me"))
    peer = parse_uint64(data.get("peer"))
    password = data.get("password")

    if me is None:
        return error_response("invalid me", 400)
    if peer is None:
        return error_response("invalid peer", 400)
    if not isinstance(password, str):
        return error_response("password required", 401)

    with state_lock:
        me_user = get_user_by_id_unlocked(me)

        if me_user is None:
            return error_response("current user not found", 404)
        if not verify_password_unlocked(me_user, password):
            return error_response("invalid password", 401)

        peer_user = get_user_by_id_unlocked(peer)
        if peer_user is None:
            return error_response("peer user not found", 404)

        result = [
            public_message(m) for m in state["messages"]
            if (m["from_id"] == me and m["to_id"] == peer)
            or (m["from_id"] == peer and m["to_id"] == me)
        ]
        result.sort(key=lambda item: item["id"])

        return ok_response({"messages": result, "total_count": len(result)})


if DEBUG_MODE:
    @app.get("/debug/state")
    def debug_state():
        with state_lock:
            return jsonify(state)

    @app.delete("/debug/reset")
    def debug_reset():
        global state
        with state_lock:
            state = default_state()
            save_state(state)
        return ok_response({"message": "state reset"})


if __name__ == "__main__":
    cert = Path("cert.pem")
    key = Path("key.pem")

    if not cert.exists() or not key.exists():
        print("ERROR: cert.pem or key.pem not found.")
        print("Generate a self-signed certificate with:")
        print("  openssl req -x509 -newkey rsa:4096 -keyout key.pem -out cert.pem -days 365 -nodes")
        exit(1)

    app.run(
        host="0.0.0.0",
        port=5000,
        ssl_context=(str(cert), str(key)),
        debug=DEBUG_MODE
    )
