#pragma once

#include <screens/i_fabric.hpp>
#include <app/app_controller.hpp>

namespace screen
{
    class ChatsFabric : public IFabric
    {
    public:
        explicit ChatsFabric(app::AppController& controller);
        ~ChatsFabric() override = default;
        ChatsFabric(const ChatsFabric&) = delete;
        ChatsFabric& operator=(const ChatsFabric&) = delete;
        ChatsFabric(ChatsFabric&&) = delete;
        ChatsFabric& operator=(ChatsFabric&&) = delete;

        ftxui::Component createScreen(int &tab_index) override;
    private:
        app::AppController& controller_;
        int inner_chat_;
        std::string new_chat_, message_;
        stx::err::Error error_;
        std::vector<ftxui::Component> chat_list_;
        std::vector<ftxui::Component> create_chat_list();
    };

}
