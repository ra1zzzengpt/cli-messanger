#include <network/request_controller/request_controller.hpp>
#include <network/request_controller/http_response.hpp>
#include <expected>
#include <curl/curl.h>
#include <utils/error/app_error.hpp>
#include <utils/logger/logs.hpp>
#include <network/request_controller/curl_raii/curl_raii.hpp>

namespace net::curl
{
    namespace
    {
        size_t WriteCallback(const char* ptr, const size_t size, const size_t nmemb, void* userdata) {
            const size_t real_size = size * nmemb;
            if (auto* buffer = static_cast<std::string*>(userdata)) {
                buffer->append(ptr, real_size);
                return real_size;
            }
            return 0;
        }

        stx::err::NetworkError statusToNetworkError(const long code) {
            switch (code) {
                case 400: return stx::err::NetworkError::BadRequest;
                case 401: return stx::err::NetworkError::Unauthorized;
                case 404: return stx::err::NetworkError::NotFound;
                case 409: return stx::err::NetworkError::Conflict;
                default:
                    if (code >= 500) return stx::err::NetworkError::ServerError;
                    return stx::err::NetworkError::UnexpectedStatus;
            }
        }

        std::string statusToString(const long code) {
            switch (code) {
                case 400: return "bad request";
                case 401: return "unauthorized";
                case 404: return "not found";
                case 409: return "conflict";
                default:
                    if (code >= 500) return "server error";
                    return "unknown status";
            }
        }

        std::expected<void, stx::err::Error> ParseResponse(const std::string& buffer, HttpResponse& response) {
            try {
                if (!buffer.empty())
                    response.data = nlohmann::json::parse(buffer);
            } catch (const nlohmann::json::parse_error& e) {
                stx::log::error("JSON parse error: " + std::string(e.what()));
                return std::unexpected(stx::err::Error{
                    stx::err::JsonError::ParsingFailed,
                    "JSON parse error: " + std::string(e.what())
                });
            }
            return {};
        }

        std::string methodToString(const RequestMethod method) {
            switch (method) {
                case RequestMethod::GET:   return "GET";
                case RequestMethod::POST:  return "POST";
                case RequestMethod::PATCH: return "PATCH";
            }
            return "UNKNOWN";
        }
    }

    std::expected<HttpResponse, stx::err::Error> RequestController::request(
            const RequestMethod& method,
            const std::string& url,
            const std::string& json_body,
            const std::vector<std::string>& custom_headers
        ) {
            HttpResponse response;
            CurlRAII curl_raii;
            if (!curl_raii.handle()) {
                stx::log::error("curl_easy_init failed for " + url);
                return std::unexpected(stx::err::Error{
                    stx::err::NetworkError::CurlInitFailed, "curl_easy_init failed"
                });
            }

            stx::log::info("sending HTTP " + methodToString(method) + " " + url);

            std::string buffer;
            curl_easy_setopt(curl_raii.handle(), CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl_raii.handle(), CURLOPT_SSL_VERIFYPEER, 0L); // ONLY IN DEVELOPMENT BEFORE ACCEPT TLS CERTIFICATE ON OFFICIAL SERVER TODO FIX
            curl_easy_setopt(curl_raii.handle(), CURLOPT_SSL_VERIFYHOST, 0L); // ONLY IN DEVELOPMENT BEFORE ACCEPT TLS CERTIFICATE ON OFFICIAL SERVER TODO FIX
            curl_easy_setopt(curl_raii.handle(), CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl_raii.handle(), CURLOPT_WRITEDATA, &buffer);
            curl_easy_setopt(curl_raii.handle(), CURLOPT_TIMEOUT, 5L);

            for (const std::string& header : custom_headers)
            {
                curl_raii.addHeader(header);
            }

            if (method == RequestMethod::POST || method == RequestMethod::PATCH) {
                curl_raii.addHeader("Content-Type: application/json");
                curl_easy_setopt(curl_raii.handle(), CURLOPT_POSTFIELDS, json_body.c_str());
                if (method == RequestMethod::PATCH)
                    curl_easy_setopt(curl_raii.handle(), CURLOPT_CUSTOMREQUEST, "PATCH");
            }

            if (curl_raii.headers())
            {
                curl_easy_setopt(curl_raii.handle(), CURLOPT_HTTPHEADER, curl_raii.headers());
            }

            if (const CURLcode res = curl_easy_perform(curl_raii.handle()); res != CURLE_OK) {
                if (res == CURLE_OPERATION_TIMEDOUT) {
                    stx::log::warn("request " + methodToString(method) + " " + url + " timed out");
                    return std::unexpected(stx::err::Error{
                        stx::err::NetworkError::Timeout, "timed out"
                    });
                }
                stx::log::error("connection failed for " + methodToString(method) + " " + url +
                    " (curl: " + std::string(curl_easy_strerror(res)) + ")");
                return std::unexpected(stx::err::Error{
                    stx::err::NetworkError::ConnectionFailed, "connection failed"
                });
            }

            curl_easy_getinfo(curl_raii.handle(), CURLINFO_RESPONSE_CODE, &response.status_code);
            stx::log::info("HTTP " + methodToString(method) + " " + url + " -> " +
                std::to_string(response.status_code));

            if (const auto parsed = ParseResponse(buffer, response); !parsed.has_value()) {
                return std::unexpected(parsed.error());
            }
            return response;
        }

        std::unexpected<stx::err::Error> RequestController::httpErr(const HttpResponse& response) {
            stx::log::warn("server returned non-OK status " + std::to_string(response.status_code) +
                " (" + statusToString(response.status_code) + ")");
            return std::unexpected(stx::err::Error{
                statusToNetworkError(response.status_code), statusToString(response.status_code)
            });
        }
}