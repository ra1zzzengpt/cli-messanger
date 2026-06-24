cmake_minimum_required(VERSION 3.20)
project(cli_messanger VERSION 0.8 LANGUAGES CXX)

cmake_policy(SET CMP0135 NEW)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(BUILD_TESTS "Build tests" ON)

include(FetchContent)

FetchContent_Declare(
        libsodium_cmake
        GIT_REPOSITORY https://github.com/robinlinden/libsodium-cmake.git
        GIT_TAG        cfebfd3da486d5a86c644c8b47067e5411c7599c
)

FetchContent_Declare(
        nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json
        GIT_TAG        v3.11.3
)

FetchContent_Declare(
        ftxui
        GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI
        GIT_TAG        v7.0.0
)

FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)

find_package(CURL QUIET)

if(NOT CURL_FOUND)
    message(FATAL_ERROR
            "[FATAL ERROR]: CURL not found. Install it manually:\n"
            "  Ubuntu:     sudo apt install libcurl4-openssl-dev\n"
            "  Arch-based: sudo pacman -S curl"
    )
endif()

set(gtest_force_shared_crt  ON  CACHE BOOL "" FORCE)
set(SODIUM_DISABLE_TESTS    ON  CACHE BOOL "" FORCE)

set(JSON_BuildTests         OFF CACHE INTERNAL "")
set(JSON_Install            OFF CACHE INTERNAL "")

set(FTXUI_BUILD_EXAMPLES    OFF CACHE BOOL "" FORCE)
set(FTXUI_BUILD_DOCS        OFF CACHE BOOL "" FORCE)

message("[FETCHING]:\n"
               "+ LibSodium-cmake - https://github.com/robinlinden/libsodium-cmake.git (cfebfd3da486d5a86c644c8b47067e5411c7599c)\n"
               "+ nlohmann-json - https://github.com/nlohmann/json (v3.11.3)\n"
               "+ FTXUI - https://github.com/ArthurSonzogni/FTXUI (v7.0.0)")
FetchContent_MakeAvailable(libsodium_cmake nlohmann_json ftxui)

if(BUILD_TESTS)
    message("[ADDING FETCH]:\n"
                   "+ google-test - https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip")
    FetchContent_MakeAvailable(googletest)
endif()
message("[DONE]")

if(BUILD_TESTS)
    enable_testing()
endif()

add_subdirectory(client)

if(BUILD_TESTS)
    add_subdirectory(tests)
endif()