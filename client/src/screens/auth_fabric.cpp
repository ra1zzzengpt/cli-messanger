#include <screens/auth_fabric.hpp>
#include <ftxui/component/component.hpp>

namespace screen
{
    AuthFabric::AuthFabric(app::AppController& controller) : controller_(controller){}

    ftxui::Component AuthFabric::createScreen()
    {
        // ----------------------- LOGIN --------------------------------
        ftxui::Element login_label_text = ftxui::text("LOGIN");

        ftxui::InputOption login_id_input_option;
        login_id_input_option.content = &id_;
        login_id_input_option.multiline = false;
        login_id_input_option.placeholder = "your id for login...";

        ftxui::Component login_id_input = ftxui::Input(login_id_input_option);

        ftxui::InputOption login_password_input_option;
        login_password_input_option.content = &password_;
        login_password_input_option.multiline = false;
        login_password_input_option.placeholder = "your password for login...";
        login_password_input_option.password = true;

        ftxui::Component login_password_input = ftxui::Input(login_password_input_option);

        ftxui::ButtonOption login_button_option;
        login_button_option.label = "->";
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
              
        };
    }
}
