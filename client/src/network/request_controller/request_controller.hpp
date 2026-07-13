#pragma once

#include <expected>
#include <string>
#include <vector>
#include <utils/error/app_error.hpp>
#include <network/request_controller/http_response.hpp>

namespace net::curl
{
    enum class RequestMethod { GET, POST, PATCH };

    class RequestController
    {
    public:
        RequestController() = delete;
        ~RequestController() = default;

        static std::expected<HttpResponse, stx::err::Error> request(const RequestMethod& method,
            const std::string& url,
            const std::string& json_body = "",
            const std::vector<std::string>& custom_headers = {});

        static std::unexpected<stx::err::Error> httpErr(const HttpResponse& response);
    };
}
