<div align="center">

# CLI Messenger

![C++](https://img.shields.io/badge/C++-20-blue?style=for-the-badge&logo=c%2B%2B)
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

**CLI Messenger** is a console application written in C++20 with a Python Flask backend. It demonstrates how to separate network logic, data storage, user interfaces, and core application flow.

The application supports two API modes:
- **HttpMessageApi** — real networking mode, connects to the Python server over HTTP via `libcurl`.
- **FakeMessageApi** — offline mode for UI/UX testing without a backend.

> **Note:** The project is currently in active development. Encryption, HTTPS, and secure credential storage are planned for a future release.

---

## Features

### Authentication & Profiles
- Password-based registration and login.
- Auto-login using local credential storage (`save.json`).
- Profile management — update nickname and password from the CLI.

### Messaging & Networking
- Incremental message fetching to reduce overhead.
- **Full Chat Dumps** — export an entire conversation to a local text file.
- Built-in server health check (`/ping` endpoint) to verify connectivity on launch.

### General UI & I/O
- Modular screen-based navigation (`AuthScreen`, `MainScreen`, `ChatScreen`, etc.).
- ASCII art menus loaded from asset files.
- Custom console utilities for safe user input with built-in validation and ANSI color support.
- Configuration stored in a local JSON file.

---

## Project Structure

```text
cli-messanger/
├── client/
│   ├── assets/
│   │   ├── menu/
│   │   └── save/
│   │       └── save.json
│   └── src/
│       ├── api/
│       │   └── message_api/
│       │       ├── fakeapi/
│       │       ├── httpapi/
│       │       └── imessage_api.h
│       ├── app/
│       ├── models/
│       ├── screens/
│       ├── utils/
│       │   ├── command/
│       │   ├── console/
│       │   └── files/
│       └── main.cpp
├── server/
│   └── server.py
├── CMakeLists.txt
├── README.md
├── LICENCE.md
└── vcpkg.json
```

## Architecture

The project is split into several independent layers.

#### `api`
Abstracts the network layer. The `IMessageApi` interface ensures the rest of the application does not depend on whether data comes from a local mock (`FakeMessageApi`) or a real server (`HttpMessageApi`).

#### `app`
Contains `AppController`, which coordinates the application lifecycle, manages the configuration, and bridges UI screens with the API layer via dependency injection.

#### `models`
Lightweight data structures (`UserInfo`, `Message`, `ChatInfo`, `AppConfig`, `ServerInfo`) serialized and deserialized using `nlohmann/json`.

#### `screens`
Handles console rendering and user interaction. Each screen (`Auth`, `Main`, `Chats`, `Profile`) is an isolated class inheriting from `IScreen`.

#### `utils`
Reusable helpers: file I/O, chat history export, console formatting, and command parsing.

#### `server`
A lightweight Python backend using Flask that manages a thread-safe dictionary of users and messages, persisted to `server_state.json`.

---

## Build

#### Requirements
- CMake 3.2+
- A compiler with C++20 support
- libcurl development headers
- Python 3.10+ with Flask

#### Client (command line)
```bash
cmake -S . -B cmake-build-debug
cmake --build cmake-build-debug
```

#### Server
```bash
cd server
pip install flask
python server.py
```

---

## Example Workflow

1. Start the Flask server.
2. Launch the client: `./cmake-build-debug/cli_messanger`
3. At the Authorization screen, register a new account. Credentials are saved to `client/assets/save/save.json`.
4. On subsequent launches, the client pings the server and logs in automatically.
5. In the Main Menu, navigate to Chats, enter a peer's ID, and start messaging.
6. Use `/dump` inside a chat to export the full conversation history to a local file.
7. Use `/help` to view all available chat commands.

---

## Why I Built This

This project was created to practice:
- Building decoupled client-server applications.
- C++20 features and dependency injection patterns.
- Modular console application development.
- Safe file handling and persistent configuration with JSON.
- Integration of external libraries (`libcurl`, `nlohmann/json`) via CMake.

It also serves as a foundation for future improvements — HTTPS transport, password hashing, and encrypted local storage.

---

## Contributing

Contributions, ideas, and suggestions are welcome.

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Commit and push
5. Open a pull request

Read `CONTRIBUTING.md` before opening a pull request.

---

## License

This project is licensed under the AGPL-3.0 License. See `LICENCE.md` for details.
