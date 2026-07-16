# Contributing to CLI Messenger

Thank you for helping improve CLI Messenger. This document describes the current codebase, development setup, conventions, and pull-request expectations. Product installation and usage belong in [README.md](README.md).

## Development setup

The client requires CMake 3.20+, a C++23 compiler, Git, and libcurl development files. CMake fetches pinned versions of FTXUI, nlohmann/json, libsodium-cmake, and GoogleTest.

```bash
git clone https://github.com/ra1zzzengpt/cli-messanger.git
cd cli-messanger
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTS=ON
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

Run the client with the repository's `build` directory as the working directory:

```bash
cd build
./client/cli_messanger
```

Platform-specific dependency instructions are in the README. When debugging client-server changes, start the Python server separately from `server/python-server/`.

## Repository map

```text
cli-messanger/
├── client/
│   ├── assets/                       runtime menus, logs, dumps, and save data
│   └── src/
│       ├── app/                      application use cases and coordination
│       ├── models/                   JSON-serializable data types
│       ├── network/
│       │   ├── api/                  messenger and GitHub API clients
│       │   ├── network_controller/   network abstraction used by the app layer
│       │   └── request_controller/   libcurl transport and RAII wrappers
│       ├── screens/                  FTXUI component factories and navigation
│       └── utils/                    crypto, persistence, errors, logging, time
├── server/python-server/server.py    Flask HTTPS server and JSON persistence
├── tests/                            GoogleTest unit tests
├── screenshots/                      README images
└── CMakeLists.txt                    root build and fetched dependencies
```

## Current architecture

The client follows a layered design:

```text
FTXUI fabrics
      ↓
AppController
      ↓
INetworkController / IStorageController / ITimeController
      ↓
network API + libcurl       encrypted local storage
      ↓
Python HTTPS server
```

### Screens

`client/src/screens/` contains FTXUI-oriented `*Fabric` classes. A fabric owns the state needed by its view and exposes `build(...)`, which returns an FTXUI component. `FabricBuilder` creates the screens, places them in an FTXUI tab container, owns the fullscreen event loop, and starts the periodic chat updater.

Screens may validate input and present errors, but network and persistence operations must go through `AppController`. Do not construct network or storage implementations inside a screen.

The screen code currently combines component creation, styling, event handlers, and some presentation logic. Separating reusable decorators, widgets, and screen layout builders is a welcome refactor as long as behavior remains covered and navigation stays centralized.

### Application layer

`AppController` is the boundary used by the UI. It coordinates network calls with local configuration updates and depends on interfaces injected from `main.cpp`:

- `INetworkController`
- `IStorageController`
- `ITimeController`

Keep FTXUI types out of this layer. If a user action needs both a server mutation and a local update, the orchestration belongs here.

### Network layer

`NetworkController` implements `INetworkController` and delegates protocol work to `MessangerApi` or `GithubApi`. `RequestController` owns common libcurl setup, HTTP status mapping, timeouts, response parsing, and headers. `CurlRAII` owns the CURL handle and `curl_slist` resources.

When adding an endpoint:

1. Add or update the operation in `INetworkController`.
2. Implement the forwarding method in `NetworkController`.
3. Implement request serialization and response validation in `MessangerApi`.
4. Add the corresponding Flask route in `server/python-server/server.py`.
5. Add tests for parsing or pure application behavior where practical.
6. Update the API table below if the public contract changed.

Client and server JSON formats must change together. Return `std::expected<T, stx::err::Error>` for recoverable C++ failures and validate a response before reading nested fields.

### Models and JSON

Models in `client/src/models/` contain data and nlohmann/json conversion functions. Keep UI, filesystem, and network behavior out of them.

Transmit `std::uint64_t` IDs as JSON strings, for example `"id": "42"`. This keeps the protocol safe for consumers whose numeric representation cannot exactly hold every 64-bit integer. Deserializers may accept both strings and numbers when backward compatibility is useful.

### Storage and crypto

`StorageController` serializes `AppConfig`, encrypts it through `CryptoSodium`, and writes a binary file. The derived encryption key is tied to the current machine identifier. All local configuration writes must go through `IStorageController`; do not write credentials or configuration as plaintext.

The runtime asset and save paths are currently relative to the repository layout. Changes in this area should move toward a platform-appropriate per-user data directory and must include migration/error handling for an existing `save.bin`.

### Server

The Flask server uses a process-local lock and persists its complete state to `server_state.json` after mutations. Passwords are bcrypt hashes; message bodies are plaintext. The development process terminates with the server, and Flask's built-in server is not a production deployment target.

Keep validation and HTTP status behavior consistent:

- `400` for malformed input
- `401` for failed credentials
- `404` for missing users/resources
- `409` for conflicts such as an existing ID
- `2xx` with `{ "ok": true, ... }` for success
- errors with `{ "ok": false, "error": "..." }`

## Server API

| Method | Path | Purpose |
|---|---|---|
| `GET` | `/ping` | Server status and uptime. |
| `GET` | `/health` | Basic service health. |
| `POST` | `/users/register` | Register `id`, `nick`, and `password`. |
| `POST` | `/users/login` | Verify `id` and `password`. |
| `POST` | `/users/get` | Resolve a public nickname by ID. |
| `PATCH` | `/users/<id>/nick` | Change a nickname after password verification. |
| `PATCH` | `/users/<id>/password` | Change a password after old-password verification. |
| `POST` | `/messages/send` | Validate the sender and store a message. |
| `POST` | `/messages/dump` | Return the conversation between two users. |

With `DEBUG_MODE=1`, the development server also exposes `GET /debug/state` and `DELETE /debug/reset`. Never enable these endpoints on an externally reachable deployment.

## C++ conventions

- Use C++23 and standard-library facilities where they clarify ownership or error handling.
- Classes and types use `PascalCase`.
- Functions and methods use `camelCase`.
- Local variables use `snake_case`.
- Private members use a trailing underscore, for example `controller_`.
- Prefer automatic storage and `std::unique_ptr`; do not add owning raw pointers or manual `new`/`delete`.
- Use RAII for handles, files, threads, CURL resources, and synchronization.
- Use `std::expected` for expected operational failures rather than exceptions.
- Mark methods and return values `const`, `noexcept`, and `[[nodiscard]]` where the contract warrants it.
- Headers use `#pragma once` and include what they directly require.
- Avoid global mutable state in the client.

Match the surrounding code when a broad naming cleanup is outside the scope of the contribution. Mechanical renames should be isolated from behavioral changes.

## UI contributions

For a new screen or reusable UI element:

1. Put full screens in `client/src/screens/` and reusable components/decorators in a clearly named subdirectory.
2. Keep persistent view state in the owning fabric, not in temporary render lambdas.
3. Capture references in callbacks only when their lifetime is guaranteed by the FTXUI component tree.
4. Route screen transitions through the shared tab index and `kScreen` values.
5. Protect data shared with the updater thread and post an FTXUI custom event after background changes.
6. Keep network requests outside render functions; rendering may happen frequently.
7. Check layouts in a small terminal and on Windows Terminal, Linux terminals, and macOS Terminal when possible.

## Tests

Tests use GoogleTest and live in `tests/`. Add source files to `tests/CMakeLists.txt`. Prioritize deterministic units such as:

- command parsing and validation;
- model JSON round trips and malformed input;
- HTTP response parsing and error mapping;
- crypto serialization boundaries;
- controller behavior with interface fakes.

Avoid tests that depend on the public internet. Server endpoint tests should use Flask's test client and temporary state rather than a long-running real server.

Before opening a pull request, run:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTS=ON
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

Also build a release configuration for changes involving ownership, threads, or platform-specific code.

## Pull-request process

1. Create a focused branch from `main`.
2. Keep generated files, certificates, local state, logs, IDE metadata, and build directories out of the commit.
3. Keep commits small enough to review and separate formatting-only changes from behavior.
4. Explain the user-visible result, implementation choices, and verification in the pull-request description.
5. Update README screenshots and usage instructions when the interface or workflow changes.
6. Confirm that client and server protocol changes are compatible and documented.
7. Ensure the debug and release builds succeed and tests pass.

## Good next contributions

- Split screen layout, component creation, decorators, and event actions into testable units.
- Move runtime data from repository-relative paths to OS-specific user-data directories.
- Re-enable TLS certificate verification and document trusted certificate setup.
- Replace password-per-request authentication with bounded server sessions.
- Add proper server packaging, dependency locking, and production deployment guidance.
- Add integration tests for every Flask endpoint and client response shape.
- Improve updater-thread synchronization and lifecycle behavior.
- Add real-time delivery using WebSockets before attempting end-to-end encryption.

Keep changes understandable, preserve layer boundaries, and prefer an explicit error over silent failure.
