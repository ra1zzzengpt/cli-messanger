#include <screens/settings_fabric.hpp>
#include <ftxui/dom/elements.hpp>

#include "utils/low_level_utils.hpp"

namespace screen
{
    SettingsFabric::SettingsFabric(app::AppController &controller): controller_(controller), inner_tab_index_(0) {}

    ftxui::Component SettingsFabric::build(int& tab_index, ftxui::ScreenInteractive&)
    {
        // SETTINGS
        // PROFILE | SERVER | PROJECT
        // CURRENT USER INFO | CURRENT SERVER INFO | CURRENT VERSION
        // SET NICKNAME -> | NEW URL |
        // SET PASSWORD -> | RE-LOGIN ON ANOTHER SERVER |
        // BACK

        // ------------------------ P R O F I L E -----------------------------
        ftxui::InputOption nickname_input_option;
        nickname_input_option.content = &nickname_;
        nickname_input_option.placeholder = "new nickname...";
        nickname_input_option.multiline = false;

        ftxui::Component nickname_input = ftxui::Input(nickname_input_option);

        ftxui::ButtonOption nickname_button_option;
        nickname_button_option.label = "->";
        nickname_button_option.transform = [&](const ftxui::EntryState& state)
        {
            ftxui::Element element = ftxui::text(state.label) | ftxui::border;
            if (controller_.canMakeRequest() && !nickname_.empty())
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
        nickname_button_option.on_click = [&]
        {
            if (!nickname_.empty() && controller_.tryAcquireRequest())
            {
                if (stx::checkNoError(controller_.updateNickname(nickname_),error_))
                {
                    nickname_.clear();
                }
            }
        };

        ftxui::Component nickname_button = ftxui::Button(nickname_button_option);

        ftxui::InputOption password_input_option;
        password_input_option.content = &password_;
        password_input_option.placeholder = "new password...";
        password_input_option.multiline = false;

        ftxui::Component password_input = ftxui::Input(password_input_option);

        ftxui::ButtonOption password_button_option;
        password_button_option.label = "->";
        password_button_option.transform = [this](const ftxui::EntryState& state)
        {
            ftxui::Element element = ftxui::text(state.label) | ftxui::border;
            if (controller_.canMakeRequest() && !password_.empty())
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
        password_button_option.on_click = [this]
        {
            if (!password_.empty() && controller_.tryAcquireRequest())
            {
                if (stx::checkNoError(controller_.updatePassword(password_),error_))
                {
                    password_.clear();
                }
            }
        };

        ftxui::Component password_button = ftxui::Button(password_button_option);

        // -------------------------- S E R V E R ----------------------------
        ftxui::InputOption new_url_input_option;
        new_url_input_option.content = &url_;
        new_url_input_option.placeholder = "new url...";
        new_url_input_option.multiline = false;

        ftxui::Component new_url_input = ftxui::Input(new_url_input_option);

        ftxui::ButtonOption new_url_button_option;
        new_url_button_option.label = "reboot";
        new_url_button_option.transform = [this](const ftxui::EntryState& state)
        {
            ftxui::Element element = ftxui::text(state.label) | ftxui::border;
            if (controller_.canMakeRequest() && !url_.empty())
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
        new_url_button_option.on_click = [this]
        {
            if (!url_.empty() && controller_.tryAcquireRequest())
            {
                inner_tab_index_ = 1;
            }
        };

        ftxui::Component new_url_button = ftxui::Button(new_url_button_option);
        // -------------------- WARNING DIALOG -----------------------------------------
        ftxui::Element warning_label = ftxui::text("!!! WARNING !!!") | ftxui::bold | ftxui::color(ftxui::Color::Yellow);
        ftxui::Element warning_text = ftxui::text("If you continue, then all your local saves (nickname, chats, ID) will not be saved.");
        ftxui::Element warning_text2 = ftxui::text("and you will have to log in to a new server (the link to which you entered above).");
        ftxui::Element warning_text3 = ftxui::text("continue only if you know what you are doing!");

        ftxui::ButtonOption dialog_back_button_option;
        dialog_back_button_option.label = "CANCEL";
        dialog_back_button_option.on_click = [&]
        {
            inner_tab_index_ = 0;
        };

        ftxui::Component dialog_back_button = ftxui::Button(dialog_back_button_option);

        ftxui::ButtonOption dialog_continue_button_option;
        dialog_continue_button_option.label = "CONTINUE";
        dialog_continue_button_option.on_click = [this,&tab_index]
        {
            if (stx::checkNoError(controller_.ping(url_), error_))
            {
                if (stx::checkNoError(controller_.relogging(url_),error_))
                {
                    tab_index = to_int(kScreen::kAuth);
                }
            }
        };

        ftxui::Component dialog_continue_button = ftxui::Button(dialog_continue_button_option);

        ftxui::Component dialog_container = ftxui::Container::Horizontal({dialog_back_button,dialog_continue_button});

        ftxui::Component dialog_renderer = ftxui::Renderer(dialog_container,[warning_label,warning_text,warning_text2,warning_text3,dialog_container]
        {
            return ftxui::center(ftxui::vbox(ftxui::center(ftxui::vbox(warning_label,warning_text,warning_text2,warning_text3,dialog_container->Render()))) | ftxui::border);
        });

        // -------------------- SOFTWARE INFO (ELEMENT ONLY) ----------------------------
        ftxui::Element cli_messanger_label = ftxui::text("CLI-MESSANGER");
        ftxui::Element version_label = ftxui::text("Current version: " + controller_.currentVersion() + ". Lastest version " + controller_.lastestVersion() + ".");

        ftxui::Element dependencies_label = ftxui::text("Dependencies:");
        ftxui::Element nlohmann_json_label = ftxui::text("nlohmann-json: v3.11.3") | ftxui::hyperlink("https://github.com/nlohmann/json") | ftxui::bold;
        ftxui::Element libsodium_label = ftxui::text("libsodium: cfebfd3da486d5a86c644c8b47067e5411c7599c") | ftxui::hyperlink("https://github.com/google/googletest/")| ftxui::bold;
        ftxui::Element ftxui_label = ftxui::text("ftxui: v7.0.0") | ftxui::hyperlink("https://github.com/ArthurSonzogni/FTXUI") | ftxui::bold;
        ftxui::Element google_label = ftxui::text("google: v1.14.0") | ftxui::hyperlink("https://github.com/google/googletest/") | ftxui::bold;

        ftxui::ButtonOption back_button_option;
        back_button_option.label = "back";
        back_button_option.on_click = [&tab_index]
        {
            tab_index = to_int(kScreen::kHello);
        };

        ftxui::Component back_button = ftxui::Button(back_button_option);

        ftxui::Component settings_container = ftxui::Container::Vertical(
            {
                ftxui::Container::Horizontal(
                    {
                        ftxui::Container::Vertical(
                            {
                                ftxui::Container::Horizontal(
                                    {
                                        nickname_input, nickname_button
                                    }),
                                ftxui::Container::Horizontal({
                                    password_input, password_button
                                })
                            }),
                        ftxui::Container::Vertical({
                            new_url_input, new_url_button
                        })
                    }),
                back_button
            });
        ftxui::Component settings_renderer = ftxui::Renderer(settings_container,
        [this,nickname_input,nickname_button,password_input,
        password_button,cli_messanger_label,version_label,
        dependencies_label,nlohmann_json_label,libsodium_label,
        ftxui_label,google_label,back_button,new_url_button,
        new_url_input]
        {
            ftxui::Element profile_label = ftxui::text("PROFILE") | ftxui::bold;
            ftxui::Element profile_nickname = ftxui::text("Nickname: " + controller_.getAppConfig().user.nickname);
            ftxui::Element profile_id = ftxui::text("ID: " + std::to_string(controller_.getAppConfig().user.id));
            ftxui::Element profile_password = ftxui::text("Password: " + std::string("*", controller_.getAppConfig().user.password.length()));

            ftxui::Element server_label =ftxui::text("SERVER") | ftxui::bold;
            ftxui::Element server_url = ftxui::text("Current server: " + controller_.getAppConfig().server.url);

            return
            ftxui::center(ftxui::vbox(ftxui::center(ftxui::text("SETTINGS") | ftxui::bold),ftxui::hbox(
                ftxui::vbox(
                    profile_label,
                    profile_nickname,
                    profile_id,
                    profile_password,
                    ftxui::hbox(
                        nickname_input->Render() | ftxui::size(ftxui::HEIGHT, ftxui::EQUAL, 1) | ftxui::border,
                        nickname_button->Render()),
                    ftxui::hbox(
                        password_input->Render() | ftxui::size(ftxui::HEIGHT, ftxui::EQUAL, 1) | ftxui::border,
                        password_button->Render())) | ftxui::border,
                ftxui::vbox(
                    server_label,
                    server_url,
                    new_url_input->Render() | ftxui::border,
                    new_url_button->Render()) | ftxui::border,
                ftxui::center(ftxui::vbox(
                    ftxui::center(cli_messanger_label),
                    ftxui::separatorDouble(),
                    ftxui::center(ftxui::vbox(
                    version_label,
                    dependencies_label,
                    nlohmann_json_label,
                    libsodium_label,
                    ftxui_label,
                    google_label)))) | ftxui::border) | ftxui::border,
                back_button->Render(),
                ftxui::text(error_.message)
            ));
        });

        ftxui::Component settings_tabs = ftxui::Container::Tab(
            {
                settings_renderer,
                dialog_renderer
            },
            &inner_tab_index_);

        ftxui::Component settings_final_renderer = ftxui::Renderer(settings_tabs, [this,settings_renderer,dialog_renderer]
        {
            ftxui::Element element = settings_renderer->Render();
            if (inner_tab_index_ == 1)
            {
                element = ftxui::dbox(
                    element,
                    dialog_renderer->Render()) | ftxui::center | ftxui::clear_under;
            }
            return element;
        });

        return settings_final_renderer;
    }
}
