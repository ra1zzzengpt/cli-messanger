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
        src/utils/files/config_storage/storage_controller.cpp
        src/utils/crypto/crypto_sodium.cpp
        src/utils/crypto/info/crypto_info.cpp
        src/utils/logger/logs.cpp
        src/network/api/messanger_api/https_api.cpp
        src/network/api/fakeapi/fake_api.cpp
        src/screens/main_screen.cpp
        src/screens/chat_screen.cpp
        src/screens/server_screen.cpp
        src/screens/profile_screen.cpp
        src/screens/chats_screen.cpp
        src/screens/auth_screen.cpp
        src/screens/auth_fabric.cpp
        src/screens/i_fabric.hpp
        src/utils/time/time_controller.cpp
        src/utils/low_level_utils.cpp
        src/screens/hello_fabric.cpp
        src/screens/settings_fabric.cpp
        src/network/api/github_api/github_api.cpp
        src/network/network_controller/network_controller.cpp
        src/network/request_controller/request_controller.cpp
        src/network/request_controller/request_controller.hpp
        src/screens/entry_fabric.cpp
        src/screens/entry_fabric.hpp
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