#include <network/request_controller/curl_raii/curl_raii.hpp>

namespace
{
    const std::function<void(void*)> curl_deleter = [](void* ptr) { if (ptr) curl_easy_cleanup(ptr); };
    const std::function<void(void*)> curl_slist_deleter = [](void* ptr){if (ptr) curl_slist_free_all(static_cast<curl_slist*>(ptr)); };
}

namespace net::curl
{
    CurlRAII::CurlRAII() : handle_(curl_easy_init(),curl_deleter), headers_(nullptr, curl_slist_deleter) { }

    void CurlRAII::addHeader(const std::string& header)
    {
        curl_slist* old = headers_.release();
        curl_slist* updated = curl_slist_append(old, header.c_str());

        if (!updated) {
            headers_.reset(old);
            return;
        }

        headers_.reset(updated);
    }

    CURL* CurlRAII::handle() const
    {
        return handle_.get();
    }

    curl_slist *CurlRAII::headers() const
    {
        return headers_.get();
    }
}
