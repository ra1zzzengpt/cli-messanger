# 🛡️ CLI Messenger (Private & Lightweight)

![C++](https://img.shields.io/badge/C++-20-blue.svg?style=for-the-badge&logo=c%2B%2B)
![Python](https://img.shields.io/badge/Python-3.10+-yellow.svg?style=for-the-badge&logo=python)
![Status](https://img.shields.io/badge/Status-Architectural_Ready-brightgreen?style=for-the-badge)
![Last Commit](https://img.shields.io/badge/Last_Commit-2026--05--07-blue?style=for-the-badge)

A minimalist, high-performance CLI messenger built with a focus on absolute privacy. The project is currently in the "Architectural Ready" phase, featuring a robust decoupled client and a synchronization server.

## 🎯 The Vision: "Zero-Knowledge" Privacy
The core philosophy of this messenger is to move towards a **Zero-Knowledge Architecture**.
- **The Server:** Acts only as a blind relay (mailbox). It manages user registration and stores encrypted binary blobs.
- **The Data:** The server will eventually have zero insight into message content, user relationships (who talks to whom), or metadata.
- **The Client:** Handles all encryption, decryption, and identity management locally.

## 🚀 Key Architectural Features
- **Strict Decoupling:** Uses an `IMessageApi` interface allowing seamless switching between `FakeMessageApi` (for offline testing) and `HttpMessageApi` (for real networking) via Dependency Injection.
- **Screen-Based Navigation:** A structured UI system (`MainScreen`, `ChatsScreen`, `ChatScreen`, etc.) that handles state transitions cleanly.
- **Resilient I/O:** Custom console utilities for safe user input (`uint32`, `uint64`, `string`) with built-in validation and ANSI color support.
- **Config Persistence:** Local storage of user profiles, server preferences, and chat metadata in JSON format.

## 💻 System Compatibility
- 🐧 **Linux:** Native environment, full ANSI color support, optimized for `clang`/`gcc`.
- 🍏 **macOS:** Fully compatible via standard build tools.
- 🪟 **Windows:** Supported via WSL2 or MinGW. Requires a terminal that supports ANSI escape sequences (like Windows Terminal or Alacritty).

## 🛠 Tech Stack
- **Languages:** C++20 (Client), Python 3.10+ (Server).
- **Networking:** [libcurl](https://curl.se/libcurl/) (Multi-protocol file and data transfer).
- **Serialization:** [nlohmann/json](https://github.com/nlohmann/json) (Modern JSON for C++).
- **Build System:** CMake + Ninja/Make.

## 🏗 Build & Execution

### Client Setup
```bash
# Generate build files
cmake -B cmake-build-debug -G Ninja

# Build the executable
ninja -C cmake-build-debug cli_messanger

# Run
./cmake-build-debug/cli_messanger
```
### Server Setup (Development)
```
cd server
pip install flask
python server.py
```
## 🗺 Roadmap
- [x] Phase 1: UI/UX Skeleton & Screen System.
- [x] Phase 2: Abstract API Layer & Data Models.
- [x] Phase 3: FakeAPI Integration for offline logic verification.
- [ ] Phase 4: Implementation of HttpMessageApi with robust libcurl response parsing.
- [ ] Phase 5: End-to-End Encryption (E2EE) implementation.
- [ ] Phase 6: Performance-focused server rewrite (C++ or Go).

Developed for power users who value the efficiency of the command line and the sanctity of their data.