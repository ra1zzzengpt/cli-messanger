cmake_minimum_required(VERSION 3.14)
project(cli_messanger)
cmake_policy(SET CMP0135 NEW)
set(CMAKE_CXX_STANDARD 23)

add_executable(cli_messanger
        client/src/main.cpp
        client/src/app/app_controller.cpp
        client/src/app/app_controller.hpp
        client/src/utils/console/console.cpp
        client/src/utils/console/console.hpp
        client/src/api/message_api/imessage_api.h
        client/src/models/user_info.hpp
        client/src/models/message.hpp
        client/src/models/chat_info.hpp
        client/src/models/server_info.hpp
        client/src/models/app_config.hpp
        client/src/models/user_info.cpp
        client/src/utils/command/command_parser.cpp
        client/src/utils/command/command_parser.hpp
        client/src/api/message_api/httpsapi/https_message_api.cpp
        client/src/api/message_api/httpsapi/https_message_api.hpp
        client/src/screens/i_screen.hpp
        client/src/screens/main_screen.cpp
        client/src/screens/main_screen.hpp
        client/src/screens/chat_screen.cpp
        client/src/screens/chat_screen.hpp
        client/src/screens/server_screen.cpp
        client/src/screens/server_screen.hpp
        client/src/screens/profile_screen.cpp
        client/src/screens/profile_screen.hpp
        client/src/screens/chats_screen.cpp
        client/src/screens/chats_screen.hpp
        client/src/api/message_api/fakeapi/fake_message_api.cpp
        client/src/api/message_api/fakeapi/fake_message_api.hpp
        client/src/models/app_config.cpp
        client/src/models/chat_info.cpp
        client/src/models/server_info.cpp
        client/src/utils/files/config_storage/config_storage.cpp
        client/src/utils/files/config_storage/config_storage.hpp
        client/src/api/message_api/httpsapi/https_response.hpp
        client/src/screens/auth_screen.cpp
        client/src/screens/auth_screen.hpp
        client/src/utils/files/files.cpp
        client/src/utils/files/files.hpp
        client/src/utils/files/paths.hpp
        client/src/models/message.cpp
        client/src/utils/crypto/crypto_sodium.cpp
        client/src/utils/crypto/crypto_sodium.hpp
        client/src/utils/crypto/info/crypto_info.cpp
        client/src/utils/crypto/info/crypto_info.hpp
        client/src/utils/error/error_types.hpp
        client/src/utils/error/app_error.hpp
        client/src/utils/logger/logs.cpp
        client/src/utils/logger/logs.hpp
)

target_include_directories(cli_messanger PRIVATE
        ${PROJECT_SOURCE_DIR}/client/src
)

find_package(CURL REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBSODIUM REQUIRED libsodium)

target_include_directories(cli_messanger PRIVATE ${LIBSODIUM_INCLUDE_DIRS})
target_link_libraries(cli_messanger
        PRIVATE
        CURL::libcurl
        nlohmann_json::nlohmann_json
        ${LIBSODIUM_LIBRARIES}
)

# --- Tests ---
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(cli_tests
        tests/test_command_parser.cpp
        tests/test_json_models.cpp
        client/src/utils/command/command_parser.cpp
        client/src/models/user_info.cpp
        client/src/models/message.cpp
        client/src/models/chat_info.cpp
        client/src/models/server_info.cpp
        client/src/models/app_config.cpp
)

target_include_directories(cli_tests PRIVATE
        ${PROJECT_SOURCE_DIR}/client/src
)
target_include_directories(cli_tests PRIVATE ${LIBSODIUM_INCLUDE_DIRS})

target_link_libraries(cli_tests
        PRIVATE
        nlohmann_json::nlohmann_json
        GTest::gtest_main
        ${LIBSODIUM_LIBRARIES}
)

include(GoogleTest)
gtest_discover_tests(cli_tests)
