add_library(cli_core STATIC
        src/app/app_controller.cpp
        src/models/user_info.cpp
        src/models/message.cpp
        src/models/chat_info.cpp
        src/models/server_info.cpp
        src/models/app_config.cpp
        src/utils/console/console.cpp
        src/utils/command/command_parser.cpp
        src/utils/files/files.cpp
        src/utils/files/config_storage/config_storage.cpp
        src/utils/crypto/crypto_sodium.cpp
        src/utils/crypto/info/crypto_info.cpp
        src/utils/logger/logs.cpp
        src/api/message_api/httpsapi/https_message_api.cpp
        src/api/message_api/fakeapi/fake_message_api.cpp
        src/screens/main_screen.cpp
        src/screens/chat_screen.cpp
        src/screens/server_screen.cpp
        src/screens/profile_screen.cpp
        src/screens/chats_screen.cpp
        src/screens/auth_screen.cpp
        src/screens/auth_fabric.cpp
        src/screens/auth_fabric.hpp
        src/screens/i_fabric.hpp
        src/utils/time/time_controller.cpp
        src/utils/time/time_controller.hpp
        src/utils/low_level_utils.cpp
        src/utils/low_level_utils.hpp
        src/screens/hello_fabric.cpp
        src/screens/hello_fabric.h
        src/screens/settings_fabric.cpp
        src/screens/settings_fabric.h
)

target_include_directories(cli_core PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(cli_core PRIVATE
        CURL::libcurl
        nlohmann_json::nlohmann_json
        sodium
        ftxui::ftxui
)

add_executable(cli_messanger src/main.cpp)

target_link_libraries(cli_messanger PRIVATE cli_core)