#include <screens/settings_fabric.hpp>
#include <ftxui/dom/elements.hpp>

namespace screen
{
    SettingsFabric::SettingsFabric(app::AppController &controller): controller_(controller) {}

    ftxui::Component SettingsFabric::createScreen(int& tab_index)
    {
        // SETTINGS
        // PROFILE | SERVER | PROJECT
        // CURRENT USER INFO | CURRENT SERVER INFO | CURRENT VERSION
        // SET NICKNAME -> | SET URL -> |
        // SET PASSWORD -> |

        // ------------------------ P R O F I L E -----------------------------
        ftxui::InputOption nickname_input_option;
        nickname_input_option.content = &nickname_;
        nickname_input_option.placeholder = "your new nickname...";
        nickname_input_option.multiline = false;

        ftxui::Component nickname_input = ftxui::Input(nickname_input_option);

        ftxui::ButtonOption nickname_button_option;
        nickname_button_option.label = "->";
        nickname_button_option.transform = [&](const ftxui::EntryState& state)
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
        nickname_button_option.on_click = [&]
        {
            if (controller_.askForRequest(std::chrono::steady_clock::now()))
            {
                if (const std::expected<void, stx::err::Error> task = controller_.updateNickname(nickname_))
                {

                }
            }
        };

        ftxui::InputOption password_input_option;
        password_input_option.content = &password_;
        password_input_option.placeholder = "your new password...";
        password_input_option.multiline = false;

        ftxui::Component password_input = ftxui::Input(password_input_option);

        // -------------------------- S E R V E R ----------------------------

        ftxui::InputOption url_input_option;
        url_input_option.content = &url_;
        url_input_option.placeholder = "your new server url...";
        url_input_option.multiline = false;

        ftxui::Component url_input = ftxui::Input(url_input_option);


    }
}
