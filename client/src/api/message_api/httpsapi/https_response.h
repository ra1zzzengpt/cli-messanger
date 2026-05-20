#pragma once

#include <nlohmann/json.hpp>

namespace api {
    struct HttpResponse {
        long status_code = 0;
        nlohmann::json data;

        [[nodiscard]] bool is_ok() const {
            return status_code >= 200 && status_code < 300;
        }
    };
}