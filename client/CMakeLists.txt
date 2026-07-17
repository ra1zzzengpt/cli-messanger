add_library(cli_core STATIC
        src/app/app_controller.cpp
        src/models/user_info.cpp
        src/models/message.cpp
        src/models/chat_info.cpp
        src/models/server_info.cpp
        src/models/app_config.cpp
        src/utils/command/command_parser.cpp
        src/utils/files/files.cpp
        src/utils/files/config_storage/storage_controller.cpp
        src/utils/crypto/crypto_sodium.cpp
        src/utils/crypto/info/crypto_info.cpp
        src/utils/logger/logs.cpp
        src/network/api/messanger_api/https_api.cpp
        src/ui/screens/i_screen.hpp
        src/ui/screens/auth_screen.cpp
        src/utils/time/time_controller.cpp
        src/utils/low_level_utils.cpp
        src/ui/screens/hello_screen.cpp
        src/ui/screens/settings_screen.cpp
        src/network/api/github_api/github_api.cpp
        src/network/network_controller/network_controller.cpp
        src/network/request_controller/request_controller.cpp
        src/ui/screens/entry_screen.cpp
        src/ui/screens/chats_screen.cpp
        src/ui/screens/screen_runner.cpp
        src/utils/time/i_time_controller.hpp
        src/utils/files/config_storage/i_storage_controller.hpp
        src/network/network_controller/i_network_controller.hpp
        src/network/request_controller/curl_raii/curl_raii.cpp
        src/ui/components/button.hpp
        src/ui/components/input.hpp
        src/ui/elements/text.hpp
)

target_include_directories(cli_core PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(cli_core PRIVATE
        CURL::libcurl
        sodium
)

target_link_libraries(cli_core PUBLIC
        nlohmann_json::nlohmann_json
        ftxui::ftxui
)

add_executable(cli_messanger src/main.cpp)

target_link_libraries(cli_messanger PRIVATE cli_core)
