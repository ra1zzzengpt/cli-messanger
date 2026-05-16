# Contributing to CLI Messenger

Thanks for your interest. To keep the codebase consistent and the architecture clean, please read this before opening a pull request.

---

## Architecture Constraints

The project is split into strict layers. Violating these boundaries is the most common reason a PR gets rejected.

| Layer | Location | Rule |
|---|---|---|
| Screens | `client/src/screens/` | No direct network calls, no filesystem access. Use `AppController` only. |
| AppController | `client/src/app/` | Coordinates screens ↔ API ↔ config. No UI logic here. |
| API | `client/src/api/` | Always implement against `IMessageApi`. No screen dependencies. |
| Models | `client/src/models/` | Plain data + JSON serialization only. No business logic. |
| Utils | `client/src/utils/` | Reusable helpers. No screen or API dependencies. |

`AppController` is constructed in `main.cpp` with dependency injection — screens receive a reference, never construct the controller themselves.

---

## Coding Standards

### C++

- **Standard:** C++20.
- **Naming:**
  - Classes: `PascalCase` — `AppController`, `ChatScreen`
  - Methods and functions: `camelCase` — `getMessages`, `sendMessage`
  - Local variables: `snake_case` — `peer_id`, `new_chat`
  - Private members: `snake_case` with trailing underscore — `messageApi_`, `configStorage_`
- **Memory:** `std::unique_ptr` for ownership. No raw `new`/`delete`.
- **Headers:** use `#pragma once`. Include only what the header directly needs.
- **Const correctness:** mark methods `const` only if they do not change observable state (this includes state reached through pointers).

### JSON and IDs

- All `uint64_t` IDs are transmitted as **strings** in JSON (`"id": "12345"`). This prevents precision loss in Python and JavaScript environments.
- `from_json` in `models/message.cpp` accepts both string and numeric forms via the internal `parse_id` helper — keep this behavior when adding new numeric fields.
- Use `nlohmann/json` for all serialization. Do not build JSON strings manually.

### HTTP

- Use the existing `GET`, `POST`, `PATCH` helpers in `HttpMessageApi`. They handle SSL bypass, 5-second timeout, and response parsing uniformly.
- Parse responses by checking `resp.is_ok() && resp.data.value("ok", false)` before accessing fields.

---

## How to Contribute

### Adding a new screen

1. Create `new_screen.h` and `new_screen.cpp` in `client/src/screens/`.
2. Inherit from `IScreen` and implement `run()` and `printScreen()`.
3. Define the screen's menu enum inside `namespace screen` in the header.
4. Instantiate the screen from the appropriate parent screen (e.g., `MainScreen`), not from `main.cpp`.

### Adding a new API method

1. Add the pure virtual method to `IMessageApi` (`client/src/api/message_api/imessage_api.h`).
2. Implement it in `HttpMessageApi` (`httpsapi/`). Follow the string-ID rule.
3. Add a wrapper method to `AppController` — screens call the controller, not the API directly.
4. Implement the corresponding endpoint in **both** servers:
   - Python: `server/python-server/server.py` — add a `@app.route(...)` handler, persist state via `save_state(state)`.
   - Rust: `server/rust-server/src/handlers.rs` — add a handler function; register the route in `main.rs`; call `save_state(&data)` after any write.
5. Both server implementations must be kept in sync — same endpoint path, same request/response shape.

> `FakeMessageApi` (`fakeapi/`) is currently inactive (fully commented out). If you need offline testing, you can resurrect it, but it must implement the full `IMessageApi` interface.

### Adding tests

Tests live in `tests/` and are built as a separate `cli_tests` target. Add test files there; register them in `CMakeLists.txt` under the `cli_tests` executable. GoogleTest is fetched automatically.

Test only pure logic (models, parsers, utilities). Do not test screens or `AppController` — they require I/O and network that the test build does not provide.

---

## Pull Request Process

1. Branch from `main`: `git checkout -b feature/your-feature`.
2. Keep commits focused. One logical change per commit.
3. The client must compile without errors (`cmake --build build`).
4. Tests must pass: `cd build && ctest --output-on-failure`.
5. PR description must explain **what** changed and **why** — not just repeat the diff.

---

*Keep it simple. Keep it layered. Happy coding.*
