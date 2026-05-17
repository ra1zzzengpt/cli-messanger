<div align="center">

# CLI Messenger

![C++](https://img.shields.io/badge/C++-20-blue?style=for-the-badge&logo=c%2B%2B)
![Python](https://img.shields.io/badge/Python-3.10+-yellow?style=for-the-badge&logo=python)
![Rust](https://img.shields.io/badge/Rust-1.75+-orange?style=for-the-badge&logo=rust)
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

**CLI Messenger** is a console application written in C++20. The client connects to one of two interchangeable backends over HTTPS via `libcurl`. The project demonstrates layered architecture, interface-based dependency injection, modular screen navigation, and machine-bound encrypted local storage powered by libsodium.

Two server implementations are provided and are functionally equivalent:

| Server | Stack | TLS |
|---|---|---|
| Python | Flask + bcrypt | optional |
| Rust | axum 0.7 + bcrypt + rustls | required (self-signed) |

---

## Features

### Authentication & Profiles
- Password-based registration and login.
- Auto-login on startup using locally stored credentials (`save.json`).
- Profile management — update nickname and password from the CLI.
- Built-in server health check (`/ping`) on every launch.

### Messaging
- Full conversation history fetched on every request — no stale state.
- **Chat dump** — export an entire conversation to a local text file via `/dump`.
- Chat list stored locally; peers are resolved by ID from the server.

### Security
- Local config is encrypted with **libsodium** (XSalsa20-Poly1305) and persisted as Base64-encoded ciphertext with a random salt and nonce.
- The encryption key is derived from the salt and is **machine-bound** — the config cannot be decrypted on a different machine.

### UI & I/O
- Screen-based navigation: `AuthScreen`, `MainScreen`, `ChatsScreen`, `ChatScreen`, `ProfileScreen`, `ServerScreen`.
- ASCII art menus loaded from asset files.
- Safe user input with validation and ANSI color output.
- Configuration persisted in a local JSON file.

---

## Project Structure

```text
cli-messanger/
├── client/
│   ├── assets/
│   │   ├── menu/          # ASCII art menus
│   │   └── save/
│   │       └── save.json  # encrypted local config and chat list
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
│           ├── crypto/    # CryptoSodium, CryptoInfo, base64 helpers
│           └── files/     # ConfigStorage, chat export, path resolution
├── server/
│   ├── python-server/
│   │   └── server.py
│   └── rust-server/
│       └── src/
│           ├── main.rs
│           ├── handlers.rs
│           ├── models.rs
│           ├── state.rs
│           └── error.rs
├── tests/
│   ├── test_command_parser.cpp
│   └── test_json_models.cpp
├── CMakeLists.txt
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
Plain data structures serialized with `nlohmann/json`. IDs are transmitted as strings to avoid precision loss across language boundaries; `from_json` handles both string and numeric forms transparently.

#### `screens`
Each screen inherits from `IScreen` and manages one UI state. Screens read config and send requests only through `AppController`.

#### `utils`
`ConfigStorage` — loads and persists `AppConfig` to JSON. Encryption and decryption are handled transparently via the embedded `CryptoSodium` instance.  
`crypto` — `CryptoSodium` wraps libsodium's secretstream; `CryptoInfo` carries salt, nonce, and ciphertext; base64 helpers bridge binary data and JSON.  
`console` — safe typed input, ANSI color output.  
`command` — parses `/`-prefixed chat commands.  
`files` — chat history export, asset path resolution.

### Server API

Both servers expose the same REST endpoints:

| Method | Path | Description |
|---|---|---|
| `GET` | `/ping` | uptime check |
| `GET` | `/health` | service check |
| `POST` | `/users/register` | register a new user |
| `POST` | `/users/login` | verify credentials |
| `GET` | `/users/:id` | get public user info |
| `PATCH` | `/users/:id/nick` | update nickname |
| `PATCH` | `/users/:id/password` | update password |
| `POST` | `/messages/send` | send a message |
| `GET` | `/messages/dump` | fetch full conversation history |

State is persisted to `server_state.json` on every write operation.

The Rust server additionally exposes debug endpoints when `DEBUG_MODE=1`:

| Method | Path | Description |
|---|---|---|
| `GET` | `/debug/state` | dump full server state |
| `DELETE` | `/debug/reset` | reset server state |

---

## Build

### Requirements

**Client:**
- CMake 3.14+
- C++20 compiler (GCC 12+ / Clang 15+)
- libcurl development headers
- nlohmann/json (resolved via CMake)
- libsodium development headers
- GoogleTest (fetched automatically for tests)

**Python server:**
- Python 3.10+
- `pip install flask bcrypt`

**Rust server:**
- Rust 1.75+
- Self-signed TLS certificate (required — see below)

### Client

```bash
cmake -S . -B build
cmake --build build
./build/cli_messanger
```

### Tests

```bash
cmake -S . -B build
cmake --build build --target cli_tests
cd build && ctest --output-on-failure
```

### Python server

```bash
cd server/python-server
pip install flask bcrypt
python server.py
```

### Rust server

Generate a self-signed certificate first:

```bash
cd server/rust-server
openssl req -x509 -newkey rsa:4096 -keyout key.pem -out cert.pem -days 365 -nodes
cargo run --release
```

The server listens on `https://127.0.0.1:5000`.

---

## CI/CD

GitHub Actions runs on every push and pull request: installs system dependencies (`libcurl`, `nlohmann-json`, `libsodium`), configures CMake, builds, and runs the test suite.

---

## Example Workflow

1. Start either server.
2. Launch the client: `./build/cli_messanger`.
3. On first launch, enter server host, port, and your nickname — saved to `save.json` (encrypted).
4. Register a new account, then login. Credentials are stored locally for auto-login.
5. On subsequent launches the client pings the server and logs in automatically.
6. In the Chats menu, add a peer by their ID and start messaging.
7. Use `/dump` inside a chat to export the full conversation to a local file.
8. Use `/help` to see all available commands.

---

## Why I Built This

- Practice building decoupled client-server applications in C++20.
- Explore interface-based dependency injection and modular screen navigation.
- Understand safe file handling and persistent configuration with JSON.
- Learn Rust by implementing a feature-equivalent backend alongside the Python one.
- Integrate libsodium for machine-bound encrypted local storage.

---

## License

This project is licensed under the AGPL-3.0 License. See `LICENCE.md` for details.
