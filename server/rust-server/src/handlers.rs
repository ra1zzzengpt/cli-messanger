use std::collections::HashMap;
use std::time::{SystemTime, UNIX_EPOCH};

use axum::{
    extract::{Path, Query, State},
    http::StatusCode,
    response::IntoResponse,
    Json,
};
use chrono::Local;
use serde::Deserialize;
use serde_json::{json, Value};

use crate::{
    error::{AppError, Result},
    models::{parse_id, parse_id_str, Message, PublicMessage, User},
    state::{save_state, ServerData, SharedState},
};

fn now_timestamp() -> String {
    Local::now().format("%m.%d %H:%M:%S").to_string()
}

fn to_public_message(msg: &Message, users: &HashMap<String, User>) -> PublicMessage {
    let from_nick = users
        .get(&msg.from_id.to_string())
        .map(|u| u.nick.clone())
        .unwrap_or_else(|| "unknown".to_string());
    PublicMessage {
        id: msg.id.to_string(),
        from_id: msg.from_id.to_string(),
        from_nick,
        to_id: msg.to_id.to_string(),
        text: msg.text.clone(),
        created_at: msg.created_at.clone(),
    }
}

// ── Request bodies ────────────────────────────────────────────────────────────

#[derive(Deserialize)]
pub struct RegisterBody {
    pub id: Option<Value>,
    pub nick: Option<String>,
    pub password: Option<String>,
}

#[derive(Deserialize)]
pub struct LoginBody {
    pub id: Option<Value>,
    pub password: Option<String>,
}

#[derive(Deserialize)]
pub struct UpdateNickBody {
    pub password: Option<String>,
    pub nick: Option<String>,
}

#[derive(Deserialize)]
pub struct UpdatePasswordBody {
    pub old_password: Option<String>,
    pub password: Option<String>,
}

#[derive(Deserialize)]
pub struct SendMessageBody {
    pub from_id: Option<Value>,
    pub to_id: Option<Value>,
    pub password: Option<String>,
    pub text: Option<String>,
}

#[derive(Deserialize)]
pub struct MessagesQuery {
    pub me: Option<String>,
    pub peer: Option<String>,
}

// ── Handlers ──────────────────────────────────────────────────────────────────

// GET /ping
pub async fn ping(State(state): State<SharedState>) -> impl IntoResponse {
    let uptime = SystemTime::now()
        .duration_since(UNIX_EPOCH)
        .unwrap()
        .as_secs()
        - state.start_time;
    Json(json!({
        "ok": true,
        "status": "online",
        "uptime": format!("{}s", uptime),
    }))
}

// GET /health
pub async fn health() -> impl IntoResponse {
    Json(json!({"ok": true, "service": "cli-messenger-dev-server"}))
}

// POST /users/register
pub async fn register_user(
    State(state): State<SharedState>,
    Json(body): Json<RegisterBody>,
) -> Result<impl IntoResponse> {
    let user_id = parse_id(body.id.as_ref())
        .ok_or_else(|| AppError::bad_request("invalid user id"))?;

    let nick = body
        .nick
        .as_deref()
        .map(str::trim)
        .filter(|s| !s.is_empty())
        .ok_or_else(|| AppError::bad_request("invalid nick"))?
        .to_string();

    if nick.len() > 32 {
        return Err(AppError::bad_request("nick is too long"));
    }

    let password = match &body.password {
        Some(p) if p.len() >= 4 => p.clone(),
        _ => return Err(AppError::bad_request("password must be at least 4 characters")),
    };

    let mut data = state.data.lock().await;

    if data.users.contains_key(&user_id.to_string()) {
        return Err(AppError::conflict("user id already exists"));
    }

    let hashed = tokio::task::spawn_blocking(move || bcrypt::hash(password, bcrypt::DEFAULT_COST))
        .await
        .unwrap()
        .map_err(|_| AppError::bad_request("failed to hash password"))?;

    let user = User {
        id: user_id,
        nick,
        password: hashed,
        created_at: now_timestamp(),
    };

    data.users.insert(user_id.to_string(), user.clone());
    save_state(&data);

    Ok((
        StatusCode::CREATED,
        Json(json!({"ok": true, "user": user.to_public()})),
    ))
}

// POST /users/login
pub async fn login_user(
    State(state): State<SharedState>,
    Json(body): Json<LoginBody>,
) -> Result<impl IntoResponse> {
    let user_id = parse_id(body.id.as_ref())
        .ok_or_else(|| AppError::bad_request("invalid user id"))?;

    let password = body
        .password
        .ok_or_else(|| AppError::bad_request("invalid password"))?;

    let data = state.data.lock().await;
    let user = data
        .users
        .get(&user_id.to_string())
        .cloned()
        .ok_or_else(|| AppError::not_found("user not found"))?;
    drop(data);

    let stored = user.password.clone();
    let valid = tokio::task::spawn_blocking(move || bcrypt::verify(password, &stored))
        .await
        .unwrap()
        .unwrap_or(false);

    if !valid {
        return Err(AppError::unauthorized("invalid password"));
    }

    Ok(Json(json!({"ok": true, "user": user.to_public()})))
}

// GET /users/:user_id
pub async fn get_user(
    State(state): State<SharedState>,
    Path(user_id): Path<String>,
) -> Result<impl IntoResponse> {
    let id = parse_id_str(&user_id)
        .ok_or_else(|| AppError::bad_request("invalid user id"))?;

    let data = state.data.lock().await;
    let user = data
        .users
        .get(&id.to_string())
        .ok_or_else(|| AppError::not_found("user not found"))?;

    Ok(Json(json!({"ok": true, "user": user.to_public()})))
}

// PATCH /users/:user_id/nick
pub async fn update_nick(
    State(state): State<SharedState>,
    Path(user_id): Path<String>,
    Json(body): Json<UpdateNickBody>,
) -> Result<impl IntoResponse> {
    let id = parse_id_str(&user_id)
        .ok_or_else(|| AppError::bad_request("invalid user id"))?;

    let password = body
        .password
        .ok_or_else(|| AppError::unauthorized("password required"))?;

    let new_nick = body
        .nick
        .as_deref()
        .map(str::trim)
        .filter(|s| !s.is_empty())
        .ok_or_else(|| AppError::bad_request("invalid nick"))?
        .to_string();

    if new_nick.len() > 32 {
        return Err(AppError::bad_request("nick is too long"));
    }

    let data = state.data.lock().await;
    let user = data
        .users
        .get(&id.to_string())
        .cloned()
        .ok_or_else(|| AppError::not_found("user not found"))?;
    drop(data);

    let stored = user.password.clone();
    let valid = tokio::task::spawn_blocking(move || bcrypt::verify(password, &stored))
        .await
        .unwrap()
        .unwrap_or(false);

    if !valid {
        return Err(AppError::unauthorized("invalid password"));
    }

    let mut data = state.data.lock().await;
    let user = data
        .users
        .get_mut(&id.to_string())
        .ok_or_else(|| AppError::not_found("user not found"))?;
    user.nick = new_nick;
    let public = user.to_public();
    save_state(&data);

    Ok(Json(json!({"ok": true, "user": public})))
}

// PATCH /users/:user_id/password
pub async fn update_password(
    State(state): State<SharedState>,
    Path(user_id): Path<String>,
    Json(body): Json<UpdatePasswordBody>,
) -> Result<impl IntoResponse> {
    let id = parse_id_str(&user_id)
        .ok_or_else(|| AppError::bad_request("invalid user id"))?;

    let old_password = body
        .old_password
        .ok_or_else(|| AppError::bad_request("old_password required"))?;

    let new_password = match &body.password {
        Some(p) if p.len() >= 4 => p.clone(),
        _ => return Err(AppError::bad_request("password must be at least 4 characters")),
    };

    let data = state.data.lock().await;
    let user = data
        .users
        .get(&id.to_string())
        .cloned()
        .ok_or_else(|| AppError::not_found("user not found"))?;
    drop(data);

    let stored = user.password.clone();
    let valid = tokio::task::spawn_blocking(move || bcrypt::verify(old_password, &stored))
        .await
        .unwrap()
        .unwrap_or(false);

    if !valid {
        return Err(AppError::unauthorized("invalid current password"));
    }

    let hashed =
        tokio::task::spawn_blocking(move || bcrypt::hash(new_password, bcrypt::DEFAULT_COST))
            .await
            .unwrap()
            .map_err(|_| AppError::bad_request("failed to hash password"))?;

    let mut data = state.data.lock().await;
    let user = data
        .users
        .get_mut(&id.to_string())
        .ok_or_else(|| AppError::not_found("user not found"))?;
    user.password = hashed;
    let public = user.to_public();
    save_state(&data);

    Ok(Json(json!({"ok": true, "user": public})))
}

// POST /messages/send
pub async fn send_message(
    State(state): State<SharedState>,
    Json(body): Json<SendMessageBody>,
) -> Result<impl IntoResponse> {
    let from_id = parse_id(body.from_id.as_ref())
        .ok_or_else(|| AppError::bad_request("invalid from_id"))?;

    let to_id = parse_id(body.to_id.as_ref())
        .ok_or_else(|| AppError::bad_request("invalid to_id"))?;

    if from_id == to_id {
        return Err(AppError::bad_request("cannot send message to yourself"));
    }

    let password = body
        .password
        .ok_or_else(|| AppError::unauthorized("password required"))?;

    let text = body
        .text
        .as_deref()
        .map(str::trim)
        .filter(|s| !s.is_empty())
        .ok_or_else(|| AppError::bad_request("message text is empty"))?
        .to_string();

    if text.len() > 4000 {
        return Err(AppError::bad_request("message text is too long"));
    }

    let data = state.data.lock().await;
    let sender = data
        .users
        .get(&from_id.to_string())
        .cloned()
        .ok_or_else(|| AppError::not_found("sender not found"))?;
    drop(data);

    let stored = sender.password.clone();
    let valid = tokio::task::spawn_blocking(move || bcrypt::verify(password, &stored))
        .await
        .unwrap()
        .unwrap_or(false);

    if !valid {
        return Err(AppError::unauthorized("invalid password"));
    }

    let mut data = state.data.lock().await;

    if !data.users.contains_key(&to_id.to_string()) {
        return Err(AppError::not_found("receiver not found"));
    }

    let message = Message {
        id: data.next_message_id,
        from_id,
        to_id,
        text,
        created_at: now_timestamp(),
    };

    data.next_message_id += 1;
    data.messages.push(message.clone());
    let public_msg = to_public_message(&message, &data.users);
    save_state(&data);

    Ok((
        StatusCode::CREATED,
        Json(json!({
            "ok": true,
            "message_id": message.id,
            "message": public_msg,
        })),
    ))
}

// GET /messages/dump
pub async fn dump_messages(
    State(state): State<SharedState>,
    Query(params): Query<MessagesQuery>,
) -> Result<impl IntoResponse> {
    let me = params
        .me
        .as_deref()
        .and_then(parse_id_str)
        .ok_or_else(|| AppError::bad_request("invalid me"))?;
    let peer = params
        .peer
        .as_deref()
        .and_then(parse_id_str)
        .ok_or_else(|| AppError::bad_request("invalid peer"))?;

    let data = state.data.lock().await;

    if !data.users.contains_key(&me.to_string()) {
        return Err(AppError::not_found("current user not found"));
    }
    if !data.users.contains_key(&peer.to_string()) {
        return Err(AppError::not_found("peer user not found"));
    }

    let mut messages: Vec<_> = data
        .messages
        .iter()
        .filter(|m| (m.from_id == me && m.to_id == peer) || (m.from_id == peer && m.to_id == me))
        .map(|m| to_public_message(m, &data.users))
        .collect();

    messages.sort_by_key(|m| m.id.parse::<u64>().unwrap_or(0));
    let total = messages.len();

    Ok(Json(json!({
        "ok": true,
        "messages": messages,
        "total_count": total,
    })))
}

// ── Debug endpoints (только при DEBUG_MODE=1) ─────────────────────────────────

pub async fn debug_state(State(state): State<SharedState>) -> impl IntoResponse {
    let data = state.data.lock().await;
    Json(serde_json::to_value(&*data).unwrap())
}

pub async fn debug_reset(State(state): State<SharedState>) -> impl IntoResponse {
    let mut data = state.data.lock().await;
    *data = ServerData::default();
    save_state(&data);
    Json(json!({"ok": true, "message": "state reset"}))
}
