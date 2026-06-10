<div align="center">

# CLI Messenger

![C++](https://img.shields.io/badge/C++-23-blue?style=for-the-badge&logo=c%2B%2B)
![Python](https://img.shields.io/badge/Python-3.10+-yellow?style=for-the-badge&logo=python)
![CMake](https://img.shields.io/badge/CMake-3.14%2B-darkblue?style=for-the-badge&logo=cmake)
![Platform](https://img.shields.io/badge/Platform-Linux%20%7C%20Windows%20%7C%20macOS-lightgrey?style=for-the-badge)
![License](https://img.shields.io/badge/License-AGPL_3.0-orange?style=for-the-badge)
![Repo Size](https://img.shields.io/github/repo-size/ra1zzzengpt/cli-messanger?style=for-the-badge)
![Last Commit](https://img.shields.io/github/last-commit/ra1zzzengpt/cli-messanger?style=for-the-badge)
![Commit Activity](https://img.shields.io/github/commit-activity/t/ra1zzzengpt/cli-messanger?style=for-the-badge)

A minimalist console messenger built to practice decoupled client-server architecture, separation of responsibilities, and retro-terminal aesthetics.

</div>

---

## Overview

**CLI Messenger** is a console application written in C++23. The client connects to the backend over HTTPS via `libcurl`. The project demonstrates layered architecture, interface-based dependency injection, modular screen navigation, and machine-bound encrypted local storage powered by libsodium.

The server is implemented in Python (Flask + bcrypt) and runs on HTTPS using a self-signed certificate.

---

## Features

### Authentication & Profiles
- Password-based registration and login. The user chooses their own numeric ID at registration.
- Auto-login on startup using locally stored credentials (`save.bin`).
- Profile management — update nickname and password from the CLI.
- Built-in server health check (`/ping`) on every launch.

### Messaging
- Full conversation history fetched on every request — no stale state.
- **Chat dump** — export an entire conversation to a local text file via `/dump`.
- Chat list stored locally; peers are resolved by ID from the server.
- Password is sent with each mutating request — no session tokens.

### Security
- Local config is encrypted with **libsodium** (XSalsa20-Poly1305) and persisted as a raw binary blob (`save.bin`) — a flat concatenation of `nonce ‖ salt ‖ ciphertext`, with no JSON wrapper or Base64 encoding.
- The encryption key is derived from the salt and is **machine-bound** — the config cannot be decrypted on a different machine.

### UI & I/O
- Screen-based navigation: `AuthScreen`, `MainScreen`, `ChatsScreen`, `ChatScreen`, `ProfileScreen`, `ServerScreen`.
- ASCII art menus loaded from asset files.
- Safe user input with validation and ANSI color output.
- Configuration persisted as an encrypted local binary file.

---

## Project Structure

```text
cli-messanger/
├── client/
│   ├── assets/
│   │   ├── menu/          # ASCII art menus
│   │   └── save/
│   │       └── save.bin   # encrypted local config and chat list (raw binary)
│   └── src/
│       ├── api/
│       │   └── message_api/
│       │       ├── httpsapi/      # HttpMessageApi (libcurl)
│       │       ├── fakeapi/       # FakeMessageApi (offline stub)
│       │       └── imessage_api.h
│       ├── app/           # AppController
│       ├── models/        # UserInfo, Message, ChatInfo, AppConfig, ServerInfo
│       ├── screens/       # IScreen and all screen implementations
│       └── utils/
│           ├── command/   # command parser (/quit, /help, /dump, /update)
│           ├── console/   # ANSI output, safe input
│           ├── crypto/    # CryptoSodium, CryptoInfo
│           ├── error/     # AppError, error type enums
│           └── files/     # ConfigStorage, chat export, path resolution
├── server/
│   └── python-server/
│       └── server.py
├── tests/
│   ├── test_command_parser.cpp
│   └── test_json_models.cpp
├── CMakeLists.txt
├── vcpkg.json
├── README.md
├── CONTRIBUTING.md
└── LICENCE.md
```

---

## Architecture

### Client layers

#### `api`
Abstracts the network layer behind `IMessageApi`. The rest of the application has no knowledge of whether data comes from a real server or a local stub. The active implementation is `HttpMessageApi` (`httpsapi/`).

#### `app`
`AppController` is the central coordinator. It owns the API and config storage via `unique_ptr` and exposes a clean interface to the screen layer. Screens never talk to the network or filesystem directly.

#### `models`
Plain data structures serialized with `nlohmann/json`. IDs are transmitted as strings to avoid precision loss across language boundaries; `from_json` handles both string and numeric forms transparently. `ServerInfo` holds a single `url` string (e.g. `https://127.0.0.1:5000`).

#### `screens`
Each screen inherits from `IScreen` and manages one UI state. Screens read config and send requests only through `AppController`.

#### `utils`
`ConfigStorage` — serializes `AppConfig` to JSON, encrypts it, and persists the result as a raw binary file (`save.bin`); load reverses the pipeline. Encryption and decryption are handled transparently via the embedded `CryptoSodium` instance.  
`crypto` — `CryptoSodium` wraps libsodium's secretbox; `CryptoInfo` carries salt, nonce, and ciphertext, with `to_export`/`import` flattening it to and from a contiguous byte buffer for on-disk storage.  
`error` — `AppError` is a discriminated union (`std::variant`) over typed error enums (`NetworkError`, `CommandError`, `CryptoError`, etc.). Functions that can fail return `std::expected<T, E>`.  
`console` — safe typed input, ANSI color output.  
`command` — parses `/`-prefixed chat commands, returns `std::expected<Command, err::CommandError>`.  
`files` — chat history export, asset path resolution.

### Server API

| Method | Path | Auth | Description |
|---|---|---|---|
| `GET` | `/ping` | — | uptime check |
| `GET` | `/health` | — | service check |
| `POST` | `/users/register` | — | register (id, nick, password) |
| `POST` | `/users/login` | password | verify credentials |
| `POST` | `/users/get` | — | get public user info by id |
| `PATCH` | `/users/:id/nick` | password | update nickname |
| `PATCH` | `/users/:id/password` | old + new password | update password |
| `POST` | `/messages/send` | password | send a message |
| `POST` | `/messages/dump` | password | fetch full conversation history |

State is persisted to `server_state.json` on every write operation.

Debug endpoints are available when `DEBUG_MODE=1`:

| Method | Path | Description |
|---|---|---|
| `GET` | `/debug/state` | dump full server state |
| `DELETE` | `/debug/reset` | reset server state |

---

## Build

### Requirements

**Client:**
- CMake 3.14+
- C++23 compiler (GCC 13+ / Clang 17+)
- libcurl development headers
- nlohmann/json (system package or via vcpkg)
- libsodium development headers
- GoogleTest (fetched automatically for tests)

**Python server:**
- Python 3.10+
- `pip install flask bcrypt`
- Self-signed TLS certificate (see below)

### Client

```bash
cmake -S . -B build
cmake --build build
./build/cli_messanger
```

### Tests

```bash
cmake -S . -B build
cmake --build build --parallel $(nproc)
ctest --test-dir build --output-on-failure
```

### Generate a self-signed certificate

The server requires `cert.pem` and `key.pem` in its working directory:

```bash
openssl req -x509 -newkey rsa:4096 -keyout key.pem -out cert.pem -days 365 -nodes
```

### Python server

```bash
cd server/python-server
pip install flask bcrypt
python server.py
```

The server listens on `https://0.0.0.0:5000` (connect via `https://127.0.0.1:5000`).

---

## CI/CD

GitHub Actions runs on every push and pull request: installs system dependencies (`libcurl`, `nlohmann-json`, `libsodium`), configures CMake, builds, and runs the test suite.

---

## Roadmap

The project is moving away from the current "poll over HTTPS" model toward a real-time, end-to-end encrypted messenger. The steps are ordered deliberately — each one unlocks the next.

- [ ] **Add structured logging.** Introduce a logging layer on the client (and server) that records errors and key actions to a file with severity levels, timestamps, and context. Today errors only surface through `AppError`/`std::expected` at the call site and vanish — once the transport becomes asynchronous (WebSockets), debugging without a persistent log is impractical. Doing this first means every step below is observable from day one.
- [ ] **Migrate the UI to [FTXUI](https://github.com/ArthurSonzogni/FTXUI).** Replace the manual ANSI/screen-loop rendering with a proper functional TUI framework. This is the prerequisite for everything below: an event-driven UI is needed to handle incoming messages asynchronously instead of blocking on request/response.
- [ ] **Switch the transport to WebSockets / raw TCP.** Drop the request-per-action HTTPS model (`libcurl` polling, full-history refetch) in favor of a persistent connection with server push, so messages arrive in real time without re-fetching the whole conversation. Authentication moves from "password on every request" to a session established once on connect.
- [ ] **End-to-end encryption (E2EE).** Encrypt message payloads on the client so the server only ever sees ciphertext. Build on the existing libsodium integration — derive per-peer keys, exchange public keys, and encrypt before send / decrypt after receive. The server becomes a blind relay.

---

## Example Workflow

1. Generate a self-signed certificate and start the server.
2. Launch the client: `./build/cli_messanger`.
3. On first launch, enter the server URL (e.g. `https://127.0.0.1:5000`) and your nickname — saved to `save.bin` (encrypted).
4. Register a new account by choosing a numeric ID and a password. Credentials are stored locally for auto-login.
5. On subsequent launches the client pings the server and logs in automatically.
6. In the Chats menu, add a peer by their ID and start messaging.
7. Use `/dump` inside a chat to export the full conversation to a local file.
8. Use `/help` to see all available commands.

---

## Why I Built This

- Practice building decoupled client-server applications in C++23.
- Explore interface-based dependency injection and modular screen navigation.
- Understand safe file handling, JSON serialization, and persisting it as a raw encrypted binary format.
- Integrate libsodium for machine-bound encrypted local storage.

---

## License

This project is licensed under the AGPL-3.0 License. See `LICENCE.md` for details.
