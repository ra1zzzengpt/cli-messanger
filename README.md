<div align="center">

# 🛡️ CLI Messenger (Private & Lightweight)

![C++](https://img.shields.io/badge/C++-20-blue?style=for-the-badge&logo=c%2B%2B)
![Python](https://img.shields.io/badge/Python-3.10+-yellow?style=for-the-badge&logo=python)
![CMake](https://img.shields.io/badge/CMake-3.2%2B-darkblue?style=for-the-badge&logo=cmake)
![Platform](https://img.shields.io/badge/Platform-Linux%20%7C%20Windows%20%7C%20macOS-lightgrey?style=for-the-badge)
![License](https://img.shields.io/badge/License-AGPL_3.0-orange?style=for-the-badge)
![Repo Size](https://img.shields.io/github/repo-size/ra1zzzengpt/cli-messanger?style=for-the-badge)
![Last Commit](https://img.shields.io/github/last-commit/ra1zzzengpt/cli-messanger?style=for-the-badge)
![Commit Activity](https://img.shields.io/github/commit-activity/t/ra1zzzengpt/cli-messanger?style=for-the-badge)

A minimalist, high-performance console messenger built with a focus on project architecture, separation of responsibilities, and a retro-terminal aesthetic.

</div>

---

## Overview

**CLI Messenger** is a console application built to practice building decoupled client-server applications. It demonstrates how to separate network logic, data storage, user interfaces, and core application flow.

The project features a **Zero-Knowledge** vision approach where the Python Flask server acts as a blind relay, while the C++ client handles all logic, configurations, and identity management locally.

The application currently supports two main API modes:
- **HttpMessageApi:** Real networking mode connecting to the Python server via HTTP (`libcurl`).
- **FakeMessageApi:** Offline mode for UI/UX testing without needing a backend.

---

## Features

### Authentication & Profiles
- Secure password-based registration and login.
- Smart auto-login using local credential storage (`save.json`).
- Profile management (updating nicknames and passwords directly from the CLI).

### Messaging & Networking
- Incremental message fetching to reduce overhead.
- **Full Chat Dumps:** Export your entire conversation history to a local text file.
- Built-in server health checks (`/ping` endpoint) to verify connectivity before launching.

### General UI & I/O
- Modular screen-based navigation (`AuthScreen`, `MainScreen`, `ChatScreen`, etc.).
- Beautifully crafted ASCII art menus for an immersive terminal experience.
- Custom console utilities for safe user input with built-in validation and ANSI color support.
- Configuration and logs loaded/saved from local JSON files.

---

## Project Structure

```text
cli-messanger/
├── client/
│   ├── assets/
│   │   ├── menu/
│   │   │   ├── auth.txt
│   │   │   ├── chats.txt
│   │   │   ├── help.txt
│   │   │   ├── main.txt
│   │   │   ├── profile.txt
│   │   │   └── server.txt
│   │   └── save/
│   │       └── save.json
│   └── src/
│       ├── api/
│       │   └── message_api/
│       │       ├── fakeapi/
│       │       ├── httpapi/
│       │       └── imessage_api.h
│       ├── app/
│       │   ├── app_controller.cpp
│       │   └── app_controller.h
│       ├── models/
│       │   ├── app_config.h
│       │   ├── chat_info.h
│       │   ├── message.h
│       │   ├── server_info.h
│       │   └── user_info.h
│       ├── screens/
│       │   ├── auth_screen.h
│       │   ├── chat_screen.h
│       │   ├── chats_screen.h
│       │   ├── i_screen.h
│       │   ├── main_screen.h
│       │   ├── profile_screen.h
│       │   └── server_screen.h
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
The project is split into several independent layers and modules.
#### `api`
Abstracts the network layer. The IMessageApi interface ensures that the rest of the application doesn't care whether data comes from a local mock (FakeMessageApi) or a real server (HttpMessageApi).
#### `app`
Contains the AppController, which coordinates the application lifecycle, manages configurations, and bridges the UI screens with the API.
#### `models`
Lightweight data structures (User, Message, ChatInfo, Config) serialized and deserialized using nlohmann/json.
#### `screens`
Handles console rendering and user interaction. Each screen (Auth, Main, Chats, Profile) is an isolated class inheriting from IScreen.
#### `utils`
Contains reusable application helpers: file I/O operations, text dumping, console formatting, and command parsers.
#### `server`
A lightweight, stateless Python backend using Flask that manages a thread-safe dictionary of users and messages.

---

## Build
#### Requirements
- CMake 4.2+
- A compiler with C++20 support
- libcurl development headers
- Python 3.10+ (with Flask)
#### Client Setup (Command Line)
```
cmake -S . -B cmake-build-debug
cmake --build cmake-build-debug
```
---
### Example Workflow
1. Start the Flask server.
2. Launch the client: `./cmake-build-debug/cli_messanger`.
3. At the Authorization Screen, register a new account. Your credentials will be saved in `client/assets/save/save.json`.
4. On subsequent launches, the client will automatically ping the server and log you in.
5. In the Main Menu, navigate to Chats, enter a peer's ID, and start messaging.
6. Use `/dump` inside a chat to save the full conversation history to your local disk.
7. Use `/help` to view all available chat commands.

---

### Why I Built This Project
This project was created to practice:
- Building decoupled client-server applications.
- Advanced C++20 features and dependency injection.
- Modular console application development.
- Safe file handling and persistent configurations (JSON).
- Integration of external libraries (libcurl, nlohmann/json) via CMake.

It also serves as a base for future improvements such as End-to-End Encryption (E2EE) and performance-focused server rewrites (C++ or Go).

---
### Contributing
Contributions, ideas, and suggestions are welcome.
1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Commit and push them
5. Open a pull request

Read CONTRIBUTING.md before opening a pull request.

---

### License
This project is licensed under the AGPL-3.0 License.
See LICENCE.md for details.