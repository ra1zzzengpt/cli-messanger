cmake_minimum_required(VERSION 3.14)
project(cli_messanger)

set(CMAKE_CXX_STANDARD 20)

add_executable(cli_messanger
        client/src/main.cpp
        client/src/app/app_controller.cpp
        client/src/app/app_controller.h
        client/src/utils/console/console.cpp
        client/src/utils/console/console.h
        client/src/api/message_api/imessage_api.h
        client/src/models/user_info.h
        client/src/models/message.h
        client/src/models/chat_info.h
        client/src/models/server_info.h
        client/src/models/app_config.h
        client/src/models/user_info.cpp
        client/src/utils/command/command_parser.cpp
        client/src/utils/command/command_parser.h
        client/src/api/message_api/httpsapi/https_message_api.cpp
        client/src/api/message_api/httpsapi/https_message_api.h
        client/src/screens/i_screen.h
        client/src/screens/main_screen.cpp
        client/src/screens/main_screen.h
        client/src/screens/chat_screen.cpp
        client/src/screens/chat_screen.h
        client/src/screens/server_screen.cpp
        client/src/screens/server_screen.h
        client/src/screens/profile_screen.cpp
        client/src/screens/profile_screen.h
        client/src/screens/chats_screen.cpp
        client/src/screens/chats_screen.h
        client/src/api/message_api/fakeapi/fake_message_api.cpp
        client/src/api/message_api/fakeapi/fake_message_api.h
        client/src/models/app_config.cpp
        client/src/models/chat_info.cpp
        client/src/models/server_info.cpp
        client/src/utils/files/config_storage/config_storage.cpp
        client/src/utils/files/config_storage/config_storage.h
        client/src/api/message_api/httpsapi/https_response.h
        client/src/screens/auth_screen.cpp
        client/src/screens/auth_screen.h
        client/src/utils/files/files.cpp
        client/src/utils/files/files.h
        client/src/utils/files/paths.h
)

add_library(cli_lib
        client/src/models/message.cpp
)

target_include_directories(cli_messanger PRIVATE
        ${PROJECT_SOURCE_DIR}/client/src
)

target_link_libraries(cli_messanger PRIVATE cli_lib)

find_package(CURL REQUIRED)
find_package(nlohmann_json REQUIRED)

target_link_libraries(cli_messanger
        PRIVATE
        CURL::libcurl
        nlohmann_json::nlohmann_json
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

target_link_libraries(cli_tests
        PRIVATE
        nlohmann_json::nlohmann_json
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(cli_tests)
