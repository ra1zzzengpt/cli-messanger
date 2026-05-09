#pragma once

#include <filesystem>

namespace paths
{
    inline std::filesystem::path getAssetsBase()
    {
        static std::filesystem::path base = []() {
            std::filesystem::path current = std::filesystem::current_path();
            for (int i = 0; i < 4; ++i) {
                if (std::filesystem::exists(current / "client" / "assets")) {
                    return current / "client" / "assets";
                }
                if (std::filesystem::exists(current / "assets")) {
                    return current / "assets";
                }
                current = current.parent_path();
            }
            throw std::runtime_error("Не найдена папка assets! Текущая директория: " + std::filesystem::current_path().string());
        }();
        return base;
    }

    inline std::filesystem::path MAIN    = getAssetsBase() / "menu" / "main.txt";
    inline std::filesystem::path HELP    = getAssetsBase() / "menu" / "help.txt";
    inline std::filesystem::path CHATS   = getAssetsBase() / "menu" / "chats.txt";
    inline std::filesystem::path PROFILE = getAssetsBase() / "menu" / "profile.txt";
    inline std::filesystem::path SERVER  = getAssetsBase() / "menu" / "server.txt";
    inline std::filesystem::path AUTH    = getAssetsBase() / "menu" / "auth.txt";
}