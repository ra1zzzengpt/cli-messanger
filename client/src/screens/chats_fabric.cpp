#include <screens/chats_fabric.hpp>

#include "utils/low_level_utils.hpp"
#include "utils/command/command_parser.hpp"
#include <utils/files/files.hpp>
#include <utils/files/paths.hpp>

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
    ChatsFabric::ChatsFabric(app::AppController &controller) : controller_(controller), index_chat_selected_(0)
    {
        chat_list_update();
    }

    ftxui::Element ChatsFabric::messages_to_element() const
    {
        std::vector<ftxui::Element> elements;
        for (const auto& msg : messages_view_)
        {
            std::string time_str = msg.created_at.empty() ? "" : "[" + msg.created_at + "] ";
            std::string prefix = (msg.from_id == controller_.getAppConfig().user.id) ? "[You]: " : "[" + chat_list_[index_chat_selected_] + "]: ";
            ftxui::Element text = ftxui::text(prefix + time_str + msg.text);
            elements.push_back(text);
        }
        return ftxui::vbox(elements);
    }

    bool ChatsFabric::messages_update()
    {
        if (controller_.getChats().empty()) return false;
        messages_view_.clear();

        UserInfo user_info;
        user_info.id = controller_.getChats()[index_chat_selected_].peer_id;
        user_info.nickname = controller_.getAppConfig().user.nickname;
        std::lock_guard lock(error_mutex_);
        if (const std::expected<std::vector<Message>,stx::err::Error> fetching_messages = controller_.getMessages(user_info); stx::checkNoError(fetching_messages,error_))
        {
            for (const Message& msg : fetching_messages.value())
            {
                messages_view_.push_back(msg);
            }
        } else
        {
            return false;
        }
        return true;
    }

    void ChatsFabric::chat_list_update()
    {
        chat_list_.clear();
        for (const ChatInfo& chat : controller_.getChats())
        {
            chat_list_.push_back(chat.peer_nick);
        }
    }

    ftxui::Component ChatsFabric::build(int &tab_index, ftxui::ScreenInteractive& screen)
    {
        // INPUT NEW CHATS  | CHAT MESSAGE
        // BUTTON NEW CHATS |
        // LIST CHATS       | NEW MESSAGE
        // BACK
        // TODO: ALL FIX!!!!
        ftxui::InputOption new_chat_input_option;
        new_chat_input_option.content = &new_chat_;
        new_chat_input_option.multiline = false;
        new_chat_input_option.placeholder = "new chat...";

        ftxui::Component new_chat_input = ftxui::Input(new_chat_input_option);

        ftxui::ButtonOption new_chat_option;
        new_chat_option.label = "New Chat";
        new_chat_option.transform = [this](const ftxui::EntryState& state)
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
            std::lock_guard lock(error_mutex_);
            const std::expected<std::uint64_t, stx::err::Error> new_chat_id = stx::transform<std::uint64_t>(new_chat_);
            if (!stx::checkNoError(new_chat_id, error_))
            {
                return;
            }
            if (new_chat_id == controller_.getAppConfig().user.id)
            {
                error_.message = "you can't add chat with you";
                return;
            }
            const auto it = std::ranges::find_if(controller_.getChats(), [new_chat_id](const ChatInfo& chat) {
            return chat.peer_id == new_chat_id;
            });
            if (it != controller_.getChats().end())
            {
                error_.message = "chat is already exist";
                return;
            }
            if (const std::expected<std::string, stx::err::Error> user_by_id = controller_.
                    getNicknameById(new_chat_id.value()); stx::checkNoError(user_by_id, error_))
            {
                if (ChatInfo chat_info{.peer_id = new_chat_id.value(), .peer_nick = user_by_id.value()};
                    stx::checkNoError(controller_.addChat(chat_info), error_))
                {
                    new_chat_.clear();
                    chat_list_update();
                }
            }
        };

        ftxui::Component new_chat_button = ftxui::Button(new_chat_option);

        ftxui::Component chats_menu = ftxui::Menu({
            .entries = &chat_list_,
            .selected = &index_chat_selected_,
            .on_enter = []
            {
                // TODO: WHAT IS THIS???
            }});

        ftxui::Component chat_messages = scroll_content([this]()-> ftxui::Element
        {
            return messages_to_element();
        });

        ftxui::InputOption new_message_input_opt;
        new_message_input_opt.content = &message_;
        new_message_input_opt.multiline = true;
        new_message_input_opt.placeholder = "new message...";

        ftxui::Component new_message_input = ftxui::Input(new_message_input_opt);

        ftxui::ButtonOption new_message_button_option;
        new_message_button_option.label = "->";
        new_message_button_option.transform = [this](const ftxui::EntryState& state)
        {
            ftxui::Element element = ftxui::text(state.label);
            if (controller_.canMakeRequest())
            {
                if (state.focused)
                {
                    element = element | ftxui::bold;
                }
            } else
            {
                element = element | ftxui::dim;
            }
            return element;
        };
        new_message_button_option.on_click = [this,&screen]
        {
            if (controller_.tryAcquireRequest())
            {
                if (controller_.getChats().empty()) return;

                UserInfo peer_user;
                peer_user.id = controller_.getChats()[index_chat_selected_].peer_id;
                peer_user.nickname = controller_.getChats()[index_chat_selected_].peer_nick;

                if (message_.starts_with("/")) // TODO: make modal dialog for /help
                {
                    if (const std::expected<stx::Command,stx::err::Error> command = stx::parseCommand(message_); stx::checkNoError(command, error_))
                    {
                        if (command == stx::Command::Quit)
                        {
                            screen.ExitLoopClosure()();
                            return;
                        } else if (command == stx::Command::Update)
                        {
                            messages_update();
                            return;
                        } else if (command == stx::Command::Dump)
                        {
                            if (messages_update() && stx::checkNoError(
                                    stx::dumpToFile(paths::getAssetsBase() / "dump" / (peer_user.nickname + ".txt"),
                                                    messages_view_, peer_user.nickname, peer_user.id),
                                    error_) && stx::checkNoError(
                                    controller_.sendMessage(
                                        peer_user,
                                        "[SYSTEM]: " + controller_.getAppConfig().user.nickname +
                                        " DUMPED THIS CHAT!!!"), error_))
                            {
                                message_.clear();
                                return;
                            }
                        }
                    }
                }

                std::lock_guard lock(error_mutex_);

                if (stx::checkNoError(controller_.sendMessage(peer_user,message_), error_))
                {
                    message_.clear();
                }
            }
        };

        ftxui::Component new_message_button = ftxui::Button(new_message_button_option);

        ftxui::ButtonOption back_button_option;
        back_button_option.label = "back";
        back_button_option.on_click = [&tab_index]
        {
            tab_index = to_int(kScreen::kHello);
        };

        ftxui::Component back_button = ftxui::Button(back_button_option);

        ftxui::Component left_panel = ftxui::Container::Vertical({
            new_chat_input,
            new_chat_button,
            chats_menu,
            back_button
        });

        ftxui::Component right_panel = ftxui::Container::Vertical({
             chat_messages,
            ftxui::Container::Horizontal({new_message_input,new_message_button})
        });

        ftxui::Component container = ftxui::Container::Horizontal({left_panel, right_panel});

        ftxui::Component container_renderer = ftxui::Renderer(container, [this,new_chat_input,new_chat_button,chats_menu,back_button,chat_messages,new_message_input,new_message_button]
        {
            return ftxui::center(ftxui::hbox({ftxui::vbox({
            new_chat_input->Render(),
            new_chat_button->Render(),
            chats_menu->Render(),
            back_button->Render(),
            ftxui::text(error_.message) | ftxui::color(ftxui::Color::Red)}) | ftxui::border , ftxui::vbox({chat_messages->Render() | ftxui::flex ,ftxui::hbox({new_message_input->Render(),new_message_button->Render()})}) | ftxui::border}));
        });

        return container_renderer;
    }
}
