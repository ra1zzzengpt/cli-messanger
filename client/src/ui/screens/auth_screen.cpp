#include <ui/screens/auth_screen.hpp>
#include <ui/components/button.hpp>
#include <ui/components/input.hpp>
#include <ftxui/component/component.hpp>
#include <utils/logger/logs.hpp>
#include <utils/low_level_utils.hpp>
#include <random>

namespace screen
{
    AuthFabric::AuthFabric(app::AppController& controller) : controller_(controller), inner_tab_index_(0){}

    ftxui::Component AuthFabric::build(int& tab_index, ftxui::ScreenInteractive& screen)
    {
        // --------------- ALL SUBSCREEN PARTS COMPONENT OPTIONS ----------------------

        const auto go_back = [this]
        {
            inner_tab_index_ = 0;
        };

        // ---------------------------- LOGIN -------------------------------------
        ftxui::Component login_id_input = ui::cmp::numeric_input(id_, "login...");
        ftxui::Component login_password_input = ui::cmp::input(password_, "password...", true);
        ftxui::Component login_button = ui::cmp::button("login", [this,&tab_index]
        {
            if (controller_.tryAcquireRequest())
            {
                const std::expected<uint64_t, stx::err::Error> transformed_id = stx::transform<uint64_t>(id_);
                if (!stx::checkNoError(transformed_id,error_))
                {
                    return;
                }
                // transformed_id.value is safety operations
                stx::log::info("login attempt for user id=" + id_);
                if (stx::checkNoError(controller_.loginUser(transformed_id.value(), password_),error_))
                {
                    stx::log::info("login successful for user id=" + id_);
                    tab_index = to_int(kScreen::kHello);
                }
            }
        }, [this] { return controller_.canMakeRequest(); });

        ftxui::Component login_back_button = ui::cmp::button("back", go_back);

        ftxui::Component login_container = ftxui::Container::Vertical({login_id_input,login_password_input,login_button,login_back_button});

        ftxui::Component login_renderer = ftxui::Renderer(login_container, [this,login_id_input,login_password_input,login_button,login_back_button]
        {
            return ftxui::center(ftxui::vbox(ftxui::center(ftxui::text("LOGIN")), ftxui::separator(),
                                             login_id_input->Render() | ftxui::border,
                                             login_password_input->Render() | ftxui::border,
                                             ftxui::hbox(login_back_button->Render(),login_button->Render()),
                                             ftxui::text(error_.message) | ftxui::bold | ftxui::color(ftxui::Color::Red)) | ftxui::border);
        });

        // ------------------- REGISTRATION ---------------------------------------
        ftxui::Element registration_label = ftxui::text("REGISTRATION");

        ftxui::Component registration_name_input = ui::cmp::input(name_, "name...");
        ftxui::Component registration_password_input = ui::cmp::input(password_, "password...", true);
        ftxui::Component registration_password_check_input = ui::cmp::input(password_control_, "password...", true);
        ftxui::Component registration_button = ui::cmp::button("registration", [this,&tab_index]
        {
            if (controller_.tryAcquireRequest())
            {
                std::mt19937_64 gen(std::random_device{}());
                const uint64_t id = std::uniform_int_distribution<uint64_t>{}(gen);
                if (password_ != password_control_)
                {
                    error_ = stx::err::Error{stx::err::PasswordError::PasswordNotEqual, "password not equal"};
                    return;
                }

                UserInfo user;
                user.id = id;
                user.password = password_;
                user.nickname = name_;

                stx::log::info("registration attempt for user id=" + std::to_string(user.id));
                if (stx::checkNoError(controller_.registerUser(user), error_))
                {
                        stx::log::info(
                            "registration successful for user id=" +
                            std::to_string(user.id)
                        );
                        tab_index = to_int(kScreen::kHello);
                }
            }
        }, [this] { return controller_.canMakeRequest(); });

        ftxui::Component registration_button_back = ui::cmp::button("back", go_back);

        const ftxui::Component registration_container = ftxui::Container::Vertical({registration_name_input,registration_password_input,registration_password_check_input,ftxui::Container::Horizontal({registration_button,registration_button_back})});

        ftxui::Component registration_renderer = ftxui::Renderer(registration_container, [this,registration_label,registration_name_input,registration_password_input,registration_password_check_input,registration_button,registration_button_back]
        {
            return ftxui::center(ftxui::vbox(ftxui::center(registration_label), ftxui::separatorDouble(),
                registration_name_input->Render() | ftxui::border,registration_password_input->Render() | ftxui::border,
                registration_password_check_input->Render() | ftxui::border,
                ftxui::hbox(registration_button_back->Render(),registration_button->Render()),
                ftxui::text(error_.message) | ftxui::bold | ftxui::color(ftxui::Color::Red)) | ftxui::border);
        });

        // -------------------- MENU COMPONENTS -----------------------
        ftxui::Element menu_label = ftxui::text("AUTHORIZATION");

        ftxui::Component menu_button_login = ui::cmp::button("login", [this]
        {
            inner_tab_index_ = 1;
        });
        ftxui::Component menu_button_registration = ui::cmp::button("registration", [this]
        {
            inner_tab_index_ = 2;
        });
        ftxui::Component menu_button_server = ui::cmp::button("change server", [&tab_index]
        {
            tab_index = to_int(kScreen::kEntry);
        });
        ftxui::Component menu_button_exit = ui::cmp::button("exit", [&screen]
        {
            screen.ExitLoopClosure()();
        });

        const ftxui::Component menu_container = ftxui::Container::Vertical({menu_button_login, menu_button_registration, menu_button_server, menu_button_exit});

        ftxui::Component menu_renderer = ftxui::Renderer(menu_container,
        [menu_label,menu_button_login,menu_button_registration,menu_button_server,menu_button_exit]
        {
            return ftxui::center(ftxui::vbox(ftxui::center(menu_label),ftxui::separatorDouble(),
                menu_button_login->Render(),
                menu_button_registration->Render(),
                menu_button_server->Render(),
                menu_button_exit->Render()) | ftxui::border);
        });

        return ftxui::Container::Tab(
            {menu_renderer, login_renderer, registration_renderer},
            &inner_tab_index_);
    }
}
