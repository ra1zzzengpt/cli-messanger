#include "base64.h"
#include <sodium.h>
namespace utils::crypto
{
    std::string to_base64(const std::vector<unsigned char>& bytes) {
        const size_t out_max = sodium_base64_ENCODED_LEN(bytes.size(), sodium_base64_VARIANT_ORIGINAL);

        std::string out(out_max, '\0');
        sodium_bin2base64(out.data(), out.size(),
                          bytes.data(), bytes.size(),
                          sodium_base64_VARIANT_ORIGINAL);
        if (!out.empty() && out.back() == '\0') out.pop_back();
        return out;
    }

    std::optional<std::vector<unsigned char>> from_base64(const std::string& s) {
        std::vector<unsigned char> out(s.size());
        size_t bin_len = 0;

        const int32_t rc = sodium_base642bin(
            out.data(), out.size(),
            s.data(), s.size(),
            /*ignore=*/nullptr,
            &bin_len,
            /*end_pointer=*/nullptr,
            sodium_base64_VARIANT_ORIGINAL);

        if (rc != 0) return std::nullopt;

        out.resize(bin_len);
        return out;
    }
}
