#pragma once

#include <ui/screens/i_screen.hpp>
#include <app/app_controller.hpp>
#include <mutex>

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

        ftxui::Component build(int &tab_index, ftxui::ScreenInteractive& screen) override;

        bool messages_update();
    private:
        app::AppController& controller_;

        std::mutex error_mutex_;

        int index_chat_selected_;
        std::vector<std::string> chat_list_;
        std::vector<Message> messages_view_;
        std::string new_chat_, message_;
        stx::err::Error error_;

        [[nodiscard]] ftxui::Element messages_to_element() const;
        void chat_list_update();
    };

}
