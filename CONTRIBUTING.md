# Contributing to CLI Messenger

Welcome! We are excited that you want to contribute to CLI Messenger. To maintain the project's architectural integrity and code quality, please follow these guidelines.

## Architecture Overview

The project follows a layered architecture with a strict separation of concerns:

1.  **Screens (UI Layer):** Handles user input and terminal output (`client/src/screens`). Screens must not contain business logic or direct network calls.
2.  **AppController (Logic Layer):** Acts as an orchestrator between the UI and the Data/API layers (`client/src/app`). It manages the `AppConfig` and state.
3.  **MessageAPI (Data Layer):** Handles communication with the server (`client/src/api`).
    *   Always program against the `IMessageApi` interface.
    *   Concrete implementations include `HttpMessageApi` (Production) and `FakeMessageApi` (Testing/Mocking).
4.  **Models:** Plain Old C++ Objects (POCO) representing data entities (`client/src/models`).

## Coding Standards

### C++ Guidelines
*   **Standard:** C++20.
*   **Naming Conventions:**
    *   **Classes:** `PascalCase` (e.g., `AppController`).
    *   **Methods:** `camelCase` (e.g., `registerUser`).
    *   **Local Variables:** `snake_case` (e.g., `user_id`).
    *   **Private Members:** `snake_case` with a trailing underscore (e.g., `message_api_`).
*   **Memory Management:** Use Smart Pointers (`std::unique_ptr`). Avoid raw `new` and `delete`.

### Network & Data
*   **ID Handling:** All `uint64_t` IDs **must** be transmitted as **strings** in JSON payloads. This prevents precision loss when communicating with the Python server or other potential non-C++ clients.
*   **JSON:** Use the `nlohmann/json` library for serialization and deserialization.
*   **HTTP:** Use the existing helper methods (`GET`, `POST`, `PATCH`) in `HttpMessageApi` to ensure consistent timeout handling (5s) and error parsing.

## How to Contribute

### 1. Adding a New Screen
1. Create `new_screen.h` and `new_screen.cpp` in `client/src/screens`.
2. Inherit from the `IScreen` interface.
3. Register and instantiate the new screen within the appropriate menu (e.g., `MainScreen`).

### 2. Adding a New API Endpoint
1. Add the virtual method signature to `IMessageApi.h`.
2. Implement a mock version in `FakeMessageApi`.
3. Implement the real network logic in `HttpMessageApi` using the string-ID rule.
4. Update the Python server (`server/server.py`) to handle the new route and ensure it persists data to `server_state.json`.

### 3. Testing
*   Verify your changes by running the local Python server.
*   Ensure that after a server restart, your data (users, messages) remains intact in the JSON state file.

## Pull Request Process
1. Create a feature branch from `main`.
2. Ensure the code compiles without warnings.
3. Submit the PR with a clear description of the feature or bugfix.

---
*Stay idiomatic. Keep it simple. Happy coding!*