#include <screens/auth_fabric.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/event.hpp>
#include <utils/console/console.hpp>
#include <utils/logger/logs.hpp>
#include <utils/low_level_utils.hpp>
#include <random>

namespace screen
{
    AuthFabric::AuthFabric(app::AppController& controller) : controller_(controller), inner_tab_index_(0){}

    ftxui::Component AuthFabric::createScreen(int& tab_index)
    {
        // --------------- ALL SUBSCREEN PARTS COMPONENT OPTIONS ----------------------

        ftxui::ButtonOption back_button_option;
        back_button_option.label = "<--/BACK/--";
        back_button_option.on_click = [&]
        {
            inner_tab_index_ = 0;
        };

        ftxui::InputOption password_input_option;
        password_input_option.content = &password_;
        password_input_option.multiline = false;
        password_input_option.placeholder = "your password for login...";
        password_input_option.password = true;

        // ---------------------------- ELEMENTS -------------------------------------

        ftxui::Element error_text = ftxui::text(error_.message);

        // ---------------------------- LOGIN -------------------------------------

        // LOGIN SUBSCREEN
        // LABEL +
        // INPUT ID +
        // INPUT PWD +
        // LOGIN +
        // BACK +
        constexpr uint8_t kSizeLoginButtons_HEIGHT = 30; // TODO: SIZES WORK

        ftxui::Element login_label_text = ftxui::text("LOGIN");

        ftxui::InputOption login_id_input_option;
        login_id_input_option.content = &id_;
        login_id_input_option.multiline = false;
        login_id_input_option.placeholder = "your id for login...";

        ftxui::Component login_id_input = ftxui::Input(login_id_input_option) | ftxui::size(ftxui::HEIGHT, ftxui::EQUAL, kSizeLoginButtons_HEIGHT);

        login_id_input |= ftxui::CatchEvent([&](const ftxui::Event& event)
        {
            return event.is_character() && !std::isdigit(static_cast<unsigned char>(event.character()[0]));
        });

        ftxui::Component login_password_input = ftxui::Input(password_input_option);

        ftxui::ButtonOption login_button_option;
        login_button_option.label = "--/LOGIN/-->";
        login_button_option.transform = [&](const ftxui::EntryState& state)
        {
            ftxui::Element element = ftxui::text(state.label) | ftxui::border;
            if (controller_.askForRequest(std::chrono::steady_clock::now()))
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
        login_button_option.on_click = [&]
        {
            if (controller_.askForRequest(std::chrono::steady_clock::now()))
            {
                const std::expected<uint64_t, stx::err::Error> transformed_id = stx::transform<uint64_t>(id_);
                if (!stx::checkNoError(transformed_id,error_))
                {
                    return;
                }
                // transformed_id.value is safety operations
                const auto user_opt = controller_.getNicknameById(transformed_id.value());
                if (!stx::checkNoError(user_opt, error_))
                {
                    return;
                }
                if (!stx::checkNoError(controller_.setLogin(user_opt.value(), password_), error_))
                {
                    return;
                }
                stx::log::info("login attempt for user id=" + id_);
                if (stx::checkNoError(controller_.loginUser(transformed_id.value(), password_),error_))
                {
                    stx::log::info("login successful for user id=" + id_);
                    tab_index = to_int(kScreen::kHello);
                }
            }
        };

        ftxui::Component login_button = ftxui::Button(login_button_option);

        ftxui::Component login_back_button = ftxui::Button(back_button_option);

        ftxui::Component login_container = ftxui::Container::Vertical({login_id_input,login_password_input,login_button,login_back_button});

        ftxui::Component login_renderer = ftxui::Renderer(login_container,[&]
        {
            return ftxui::center(ftxui::vbox(login_label_text,login_container->Render(),error_text));
        });

        // ------------------- REGISTRATION ----------------------------------------

        // REGISTRATION SUBSCREEN
        // LABEL +
        // INPUT NAME +
        // INPUT PWD +
        // INPUT PWD2 +
        // REGISTRATION +
        // BACK +

        ftxui::Element registration_label = ftxui::text("REGISTRATION");

        ftxui::InputOption registration_name_input_option;
        registration_name_input_option.content = &name_;
        registration_name_input_option.multiline = false;
        registration_name_input_option.placeholder = "your name for registration...";

        ftxui::Component registration_name_input = ftxui::Input(registration_name_input_option);

        ftxui::Component registration_password_input = ftxui::Input(password_input_option);

        ftxui::InputOption registration_password_check_option = password_input_option;
        registration_password_check_option.content = &password_control_;

        ftxui::Component registration_password_check_input = ftxui::Input(registration_password_check_option);

        ftxui::ButtonOption registration_button_option;
        registration_button_option.label = "--/REGISTRATION/-->";
        registration_button_option.transform = [&](const ftxui::EntryState& state)
        {
            ftxui::Element element = ftxui::text(state.label) | ftxui::border;
            if (controller_.askForRequest(std::chrono::steady_clock::now()))
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
        registration_button_option.on_click = [&]
        {
            if (controller_.askForRequest(std::chrono::steady_clock::now()))
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

                if (!stx::checkNoError(controller_.updatePassword(password_), error_))
                {
                    return;
                }

                stx::log::info("registration attempt for user id=" + std::to_string(user.id));
                if (stx::checkNoError(controller_.registerUser(user), error_))
                {
                    stx::log::info("registration successful for user id=" + std::to_string(user.id));
                    tab_index = to_int(kScreen::kHello);
                }
            } else
            {

            }
        };

        ftxui::Component registration_button = ftxui::Button(registration_button_option);

        ftxui::Component registration_button_back = ftxui::Button(back_button_option);

        ftxui::Component registration_container = ftxui::Container::Vertical({registration_name_input,registration_password_input,registration_password_check_input,registration_button,registration_button_back});

        ftxui::Component registration_renderer = ftxui::Renderer(registration_container, [&]
        {
            return ftxui::center(ftxui::vbox(registration_label,registration_container->Render(),error_text));
        });

        // -------------------- MENU COMPONENTS -----------------------

        // MENU SUBSCREEN AUTH
        // LABEL +
        // BUTTON LOGIN +
        // BUTTON REGISTER +
        // BUTTON SERVER SETTINGS SCREEN
        // BUTTON EXIT +

        ftxui::Element menu_label = ftxui::text("AUTHORIZATION");

        ftxui::ButtonOption menu_button_login_option;
        menu_button_login_option.label = "login";
        menu_button_login_option.on_click = [&]
        {
            inner_tab_index_ = 1;
        };

        ftxui::Component menu_button_login = ftxui::Button(menu_button_login_option);

        ftxui::ButtonOption menu_button_registration_option;
        menu_button_registration_option.label = "registration";
        menu_button_registration_option.on_click = [&]
        {
            inner_tab_index_ = 2;
        };

        ftxui::Component menu_button_registration = ftxui::Button(menu_button_registration_option);

        ftxui::ButtonOption menu_button_server_option;
        menu_button_server_option.label = "change server";
        menu_button_server_option.on_click = [&]
        {
            tab_index = to_int(kScreen::kEntry);
        };

        ftxui::Component menu_button_server = ftxui::Button(menu_button_server_option);

        ftxui::ButtonOption menu_exit_option;
        menu_exit_option.label = "exit";
        menu_exit_option.on_click = [&]
        {
            tab_index = to_int(kScreen::kEntry);
        };

        ftxui::Component menu_button_exit = ftxui::Button(menu_exit_option);

        ftxui::Component menu_container = ftxui::Container::Vertical({menu_button_login, menu_button_registration, menu_button_server, menu_button_exit});

        ftxui::Component menu_renderer = ftxui::Renderer(menu_container, [&]
        {
            return ftxui::center(ftxui::vbox(menu_label, menu_container->Render()));
        });

        ftxui::Component auth_subscreen_tabs = ftxui::Container::Tab({menu_renderer,login_renderer,registration_renderer},&inner_tab_index_);

        ftxui::Component auth_renderer = ftxui::Renderer(auth_subscreen_tabs, [&]
        {
            ftxui::Element document;
            switch (inner_tab_index_)
            {
                case 0:
                    document = menu_renderer->Render();
                    break;
                case 1:
                    document = login_renderer->Render();
                    break;
                case 2:
                    document = registration_renderer->Render();
                    break;
                default: break;
            };
            return document;
        });

        return auth_renderer;
    }
}
