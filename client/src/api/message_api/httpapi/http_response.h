#pragma once

#include <string>
#include <nlohmann/json.hpp>

namespace api {
    struct HttpResponse {
        long status_code = 0;
        nlohmann::json data;
        std::string error_details;

        [[nodiscard]] bool is_ok() const {
            return status_code >= 200 && status_code < 300 && error_details.empty();
        }
    };
}