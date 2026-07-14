#include <screens/chats_fabric.hpp>

#include "utils/low_level_utils.hpp"

namespace
{
    ftxui::Component scroll_content(const std::function<ftxui::Element()>& get_content)
    {
        class Impl : public ftxui::ComponentBase
        {
            float scroll = 1.f;
        public:
            explicit Impl(const std::function<ftxui::Element()>& get_content)
            {
                ftxui::Component scrollable_content = ftxui::Renderer([this,get_content]
                {
                    return get_content() | ftxui::focusPositionRelative(0.f,scroll) | ftxui::yframe | ftxui::xflex;
                });
                ftxui::SliderOption<float> option;
                option.value = &scroll;
                option.min = 0.f;
                option.max = 1.f;
                option.increment = 0.1f;
                option.direction = ftxui::Direction::Down;
                option.color_active = ftxui::Color::Red;
                option.color_inactive = ftxui::Color::DarkRed;
                ftxui::Component slider = ftxui::Slider(option);

                Add(ftxui::Container::Horizontal({scrollable_content, slider}));
            }
        };
        return ftxui::Make<Impl>(get_content)
             | ftxui::size(ftxui::HEIGHT, ftxui::EQUAL, 15)
             | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 50);
    }

}

namespace screen
{
    ChatsFabric::ChatsFabric(app::AppController &controller) : controller_(controller), inner_chat_(0) {}

    std::vector<ftxui::Component> ChatsFabric::create_chat_list()
    {
        std::vector<ftxui::Component> chat_list;
        const std::vector<ChatInfo> chats = controller_.getChats();
        for (std::size_t i = 0; i < chats.size(); ++i)
        {
            ftxui::ButtonOption choose_chat_option;
            choose_chat_option.label = chats[i].peer_nick + "(" + std::to_string(chats[i].peer_id) + ")";
            choose_chat_option.on_click = [&]
            {
                inner_chat_ = static_cast<int>(i);
            };
        }
        return chat_list;
    }

    ftxui::Component ChatsFabric::createScreen(int &tab_index)
    {
        // INPUT NEW CHATS  | CHAT MESSAGE
        // BUTTON NEW CHATS |
        // LIST CHATS       | NEW MESSAGE
        // BACK
        // TODO: ALL FIX!!!!
        ftxui::InputOption new_chat_input_option;
        new_chat_input_option.content = new_chat_;
        new_chat_input_option.multiline = false;
        new_chat_input_option.placeholder = "new chat...";

        ftxui::Component new_chat_input = ftxui::Input(new_chat_input_option);

        ftxui::ButtonOption new_chat_option;
        new_chat_option.label = "New Chat";
        new_chat_option.transform = [&](const ftxui::EntryState& state)
        {
            ftxui::Element element = ftxui::text(state.label) | ftxui::border;
            if (!new_chat_.empty())
            {
                if (state.focused)
                {
                    element = element | ftxui::bold;
                }
            } else
            {
                element = element | ftxui::dim;
                if (state.focused)
                {
                    element = element | ftxui::bold;
                }
            }
            return element;
        };
        new_chat_option.on_click = [&]
        {
            const std::expected<std::uint64_t, stx::err::Error> new_chat_id = stx::transform<std::uint64_t>(new_chat_);
            if (!stx::checkNoError(new_chat_id, error_))
            {
                new_chat_.clear();
                return;
            }
            if (const std::expected<UserInfo, stx::err::Error> user_by_id = controller_.
                    getNicknameById(new_chat_id.value()); stx::checkNoError(user_by_id, error_))
            {
                if (ChatInfo chat_info{.peer_id = new_chat_id.value(), .peer_nick = user_by_id.value().nickname};
                    stx::checkNoError(controller_.addChat(chat_info), error_))
                {
                    new_chat_.clear();
                }
            }
        };

        ftxui::Component new_chat_button = ftxui::Button(new_chat_option);
    }
}
