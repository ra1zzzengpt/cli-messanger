cmake_minimum_required(VERSION 4.2)
project(cli_messanger)

set(CMAKE_CXX_STANDARD 20)

add_executable(cli_messanger client/src/main.cpp
        client/src/app/app.cpp
        client/src/app/app.h
        client/src/console/console.cpp
        client/src/console/console.h
        client/src/console/menu/main_menu.cpp
        client/src/console/menu/main_menu.h
        client/src/console/menu/server_menu.cpp
        client/src/console/menu/server_menu.h
        client/src/console/menu/profile_menu.cpp
        client/src/console/menu/profile_menu.h
        client/src/console/menu/chats_menu.cpp
        client/src/console/menu/chats_menu.h
        client/src/console/menu/chat_screen.cpp
        client/src/console/menu/chat_screen.h
        client/src/console/menu/register_menu.cpp
        client/src/console/menu/register_menu.h
        client/src/api/message_api/imessage_api.h
        client/src/models/user_info.h
        client/src/models/message.h
        client/src/models/chat_info.h
        client/src/models/server_info.h
        client/src/models/app_config.h
        client/src/models/user_info.cpp
        client/src/utils/command/command_parser.cpp
        client/src/utils/command/command_parser.h
        client/src/utils/files/json/json_parser.cpp
        client/src/utils/files/json/json_parser.h
        client/src/utils/files/file_utils.cpp
        client/src/utils/files/file_utils.h
        client/src/api/message_api/httpapi/http_message_api.cpp
        client/src/api/message_api/httpapi/http_message_api.h)

add_library(cli_lib
        client/src/models/message.cpp)

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
