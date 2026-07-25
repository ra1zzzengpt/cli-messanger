#include <ui/screens/chats_screen.hpp>
#include <ui/components/button.hpp>
#include <ui/components/input.hpp>

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
    ChatsFabric::ChatsFabric(app::AppController &controller) : controller_(controller), index_chat_selected_(0), inner_tab_index_(0)
    {
        chat_list_update();
    }

    ftxui::Element ChatsFabric::messages_to_element() const
    {
        ftxui::Elements elements;
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
        std::lock_guard lock(mutex_);
        messages_view_.clear();

        UserInfo user_info;
        user_info.id = controller_.getChats()[index_chat_selected_].peer_id;
        user_info.nickname = controller_.getAppConfig().user.nickname;
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
        ftxui::Component new_chat_input = ui::cmp::input(new_chat_, "new chat...");
        ftxui::Component new_chat_button = ui::cmp::button("New Chat", [this]
        {
            std::lock_guard lock(mutex_);
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
        }, [this] { return !new_chat_.empty(); });

        ftxui::Component chats_menu = ftxui::Menu({
            .entries = &chat_list_,
            .selected = &index_chat_selected_
        });

        ftxui::Component chat_messages = scroll_content([this]()-> ftxui::Element
        {
            return messages_to_element();
        });

        ftxui::Component new_message_input = ui::cmp::input(message_, "new message...", false, false);
        ftxui::Component new_message_button = ui::cmp::button("->", [this,&screen]
        {
            if (controller_.tryAcquireRequest())
            {
                if (controller_.getChats().empty()) return;

                UserInfo peer_user;
                peer_user.id = controller_.getChats()[index_chat_selected_].peer_id;
                peer_user.nickname = controller_.getChats()[index_chat_selected_].peer_nick;

                if (message_.starts_with("/"))
                {
                    if (const std::expected<stx::Command,stx::err::Error> command = stx::parseCommand(message_); stx::checkNoError(command, error_))
                    {
                        if (command == stx::Command::Quit)
                        {
                            screen.ExitLoopClosure()();
                        } if (command == stx::Command::Update)
                        {
                            messages_update();
                        } if (command == stx::Command::Dump)
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
                            }
                        } if (command == stx::Command::Help)
                        {
                            inner_tab_index_ = 1;
                        }
                    }
                    return;
                }

                std::lock_guard lock(mutex_);

                if (stx::checkNoError(controller_.sendMessage(peer_user,message_), error_))
                {
                    message_.clear();
                }
            }
        }, [this] { return controller_.canMakeRequest(); }, false);

        // --- DIALOG ---
        ftxui::Element help = ftxui::text("HELP");

        ftxui::Element help_info = ftxui::text("/help - this dialog,\n/update - updating messages,\n/dump - dumping messages,\n/quit - leave from messanger.");

        ftxui::Component help_back = ui::cmp::button("back",[this]
        {
            inner_tab_index_ = 0;
        });

        ftxui::Component help_renderer = ftxui::Renderer(help_back,[help,help_info,help_back]
        {
            return ftxui::center(ftxui::vbox({
            help | ftxui::bold | ftxui::center, ftxui::separatorDouble(), help_info | ftxui::border,help_back->Render()}) | ftxui::border);
        });

        ftxui::Component back_button = ui::cmp::button("back", [&tab_index]
        {
            tab_index = to_int(kScreen::kHello);
        });

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

        const ftxui::Component container = ftxui::Container::Horizontal({left_panel, right_panel});

        ftxui::Component container_renderer = ftxui::Renderer(container, [this,new_chat_input,new_chat_button,chats_menu,back_button,chat_messages,new_message_input,new_message_button]
        {
            const ftxui::Element chats_panel = ftxui::vbox({
                new_chat_input->Render(),
                new_chat_button->Render(),
                chats_menu->Render(),
                back_button->Render(),
                ftxui::text(error_.message) | ftxui::bold | ftxui::color(ftxui::Color::Red) | ftxui::center,
            }) | ftxui::border;

            const ftxui::Element messages_panel = ftxui::vbox({
                chat_messages->Render() | ftxui::flex,
                ftxui::hbox({new_message_input->Render(), new_message_button->Render()}),
            }) | ftxui::border;

            return ftxui::hbox({chats_panel, messages_panel}) | ftxui::center;
        });

        ftxui::Component container_tabs = ftxui::Container::Tab({
        container_renderer,help_renderer},&inner_tab_index_);

        ftxui::Component tab_renderer = ftxui::Renderer(container_tabs,[this,container_renderer,help_renderer]
        {
            ftxui::Element element = container_renderer->Render();
            if (inner_tab_index_ == 1)
            {
                element = ftxui::dbox(element,help_renderer->Render());
                return element;
            }
            return element;
        });

        return tab_renderer;
    }
}
