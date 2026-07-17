#include <ui/screens/settings_screen.hpp>
#include <ftxui/dom/elements.hpp>

#include <ui/components/button.hpp>
#include <ui/components/input.hpp>
#include <ui/elements/text.hpp>
#include "utils/low_level_utils.hpp"

namespace screen
{
    SettingsFabric::SettingsFabric(app::AppController &controller): controller_(controller), inner_tab_index_(0) {}

    ftxui::Component SettingsFabric::build(int& tab_index, ftxui::ScreenInteractive&)
    {
        // ------------------------ P R O F I L E -----------------------------
        ftxui::Component nickname_input = ui::cmp::input(nickname_, "new nickname...");
        ftxui::Component nickname_button = ui::cmp::button("->", [this]
        {
            if (!nickname_.empty() && controller_.tryAcquireRequest())
            {
                if (stx::checkNoError(controller_.updateNickname(nickname_),error_))
                {
                    nickname_.clear();
                }
            }
        }, [this] { return controller_.canMakeRequest() && !nickname_.empty(); });

        ftxui::Component password_input = ui::cmp::input(password_, "new password...", true);
        ftxui::Component password_button = ui::cmp::button("->", [this]
        {
            if (!password_.empty() && controller_.tryAcquireRequest())
            {
                if (stx::checkNoError(controller_.updatePassword(password_),error_))
                {
                    password_.clear();
                }
            }
        }, [this] { return controller_.canMakeRequest() && !password_.empty(); });

        // -------------------------- S E R V E R ----------------------------
        ftxui::Component new_url_input = ui::cmp::input(url_, "new url...");
        ftxui::Component new_url_button = ui::cmp::button("reboot", [this]
        {
            if (!url_.empty() && controller_.tryAcquireRequest())
            {
                inner_tab_index_ = 1;
            }
        }, [this] { return controller_.canMakeRequest() && !url_.empty(); });
        // -------------------- WARNING DIALOG -----------------------------------------
        ftxui::Element warning_label = ftxui::text("!!! WARNING !!!") | ftxui::bold | ftxui::color(ftxui::Color::Yellow);
        ftxui::Element warning_text = ftxui::text("If you continue, then all your local saves (nickname, chats, ID) will not be saved.");
        ftxui::Element warning_text2 = ftxui::text("and you will have to log in to a new server (the link to which you entered above).");
        ftxui::Element warning_text3 = ftxui::text("continue only if you know what you are doing!");

        ftxui::Component dialog_back_button = ui::cmp::button("CANCEL", [this]
        {
            inner_tab_index_ = 0;
        });
        ftxui::Component dialog_continue_button = ui::cmp::button("CONTINUE", [this,&tab_index]
        {
            if (stx::checkNoError(controller_.ping(url_), error_))
            {
                if (stx::checkNoError(controller_.relogging(url_),error_))
                {
                    tab_index = to_int(kScreen::kAuth);
                }
            }
        });

        ftxui::Component dialog_container = ftxui::Container::Horizontal({dialog_back_button,dialog_continue_button});

        ftxui::Component dialog_renderer = ftxui::Renderer(dialog_container,
        [warning_label,warning_text,warning_text2,warning_text3,dialog_back_button,dialog_continue_button]
        {
            return ftxui::center(ftxui::vbox(
                warning_label,
                warning_text,
                warning_text2,
                warning_text3,
                ftxui::hbox(dialog_back_button->Render(), dialog_continue_button->Render()) | ftxui::center
            ) | ftxui::border);
        });

        // -------------------- SOFTWARE INFO (ELEMENT ONLY) ----------------------------
        ftxui::Element cli_messanger_label = ftxui::text("CLI-MESSANGER");
        ftxui::Element version_label = ftxui::text("Current version: " + controller_.currentVersion() + ". Lastest version " + controller_.lastestVersion() + ".");

        ftxui::Element dependencies_label = ftxui::text("Dependencies:");
        ftxui::Element nlohmann_json_label = ftxui::text("nlohmann-json: v3.11.3") | ftxui::hyperlink("https://github.com/nlohmann/json") | ftxui::bold;
        ftxui::Element libsodium_label = ftxui::text("libsodium: cfebfd3da486d5a86c644c8b47067e5411c7599c") | ftxui::hyperlink("https://github.com/google/googletest/")| ftxui::bold;
        ftxui::Element ftxui_label = ftxui::text("ftxui: v7.0.0") | ftxui::hyperlink("https://github.com/ArthurSonzogni/FTXUI") | ftxui::bold;
        ftxui::Element google_label = ftxui::text("google: v1.14.0") | ftxui::hyperlink("https://github.com/google/googletest/") | ftxui::bold;

        ftxui::Component back_button = ui::cmp::button("back", [&tab_index]
        {
            tab_index = to_int(kScreen::kHello);
        });

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

            const ftxui::Element profile_panel = ftxui::vbox(
                    profile_label,
                    profile_nickname,
                    profile_id,
                    profile_password,
                    ftxui::hbox(
                        nickname_input->Render() | ftxui::size(ftxui::HEIGHT, ftxui::EQUAL, 1) | ftxui::border,
                        nickname_button->Render()),
                    ftxui::hbox(
                        password_input->Render() | ftxui::size(ftxui::HEIGHT, ftxui::EQUAL, 1) | ftxui::border,
                        password_button->Render())) | ftxui::border;

            const ftxui::Element server_panel = ftxui::vbox(
                    server_label,
                    server_url,
                    new_url_input->Render() | ftxui::border,
                    new_url_button->Render()) | ftxui::border;

            const ftxui::Element project_panel = ftxui::vbox(
                    ftxui::center(cli_messanger_label),
                    ftxui::separatorDouble(),
                    ftxui::center(ftxui::vbox(
                        version_label,
                        dependencies_label,
                        nlohmann_json_label,
                        libsodium_label,
                        ftxui_label,
                        google_label))) | ftxui::border;

            return ftxui::vbox(
                ftxui::text("SETTINGS") | ftxui::bold | ftxui::center,
                ftxui::hbox(profile_panel, server_panel, project_panel) | ftxui::border,
                back_button->Render(),
                ui::elm::error_text(error_.message))| ftxui::center;
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
                    dialog_renderer->Render()) | ftxui::center;
            }
            return element;
        });

        return settings_final_renderer;
    }
}
