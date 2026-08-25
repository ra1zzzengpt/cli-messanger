#ifndef CURL_RAII_HPP
#define CURL_RAII_HPP

#include <curl/curl.h>
#include <memory>
#include <functional>

namespace net::curl
{
    class CurlRAII
    {
    public:
        CurlRAII();
        ~CurlRAII() = default;
        CurlRAII(const CurlRAII &) = delete;
        CurlRAII(CurlRAII &&) = delete;
        CurlRAII &operator=(const CurlRAII &) = delete;
        CurlRAII &operator=(CurlRAII &&) = delete;

        void addHeader(const std::string& header);

        [[nodiscard]] CURL* handle() const;
        [[nodiscard]] curl_slist* headers() const;
    private:
        std::unique_ptr<CURL,std::function<void(void*)>> handle_;
        std::unique_ptr<curl_slist,std::function<void(void*)>> headers_;
    };
}

#endif
