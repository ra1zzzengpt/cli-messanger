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

    inline std::filesystem::path main_menu = getAssetsBase() / "menu" / "main.txt";
    inline std::filesystem::path help      = getAssetsBase() / "menu" / "help.txt";
    inline std::filesystem::path chats     = getAssetsBase() / "menu" / "chats.txt";
    inline std::filesystem::path profile   = getAssetsBase() / "menu" / "profile.txt";
    inline std::filesystem::path server    = getAssetsBase() / "menu" / "server.txt";
    inline std::filesystem::path auth      = getAssetsBase() / "menu" / "auth.txt";
}