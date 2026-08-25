#ifndef LOW_LEVEL_UTILS_HPP
#define LOW_LEVEL_UTILS_HPP

#include <expected>
#include <utils/error/app_error.hpp>
#include <limits>
#include <cstdint>
#include <stdexcept>

namespace stx
{
    // TODO: THINK ABOUT FUNCTION
    // ERROR - FALSE
    template<typename T>
    bool checkNoError(const std::expected<T, err::Error>& result, err::Error& error)
    {
        if (result.has_value()) return true;
        // print(std::string(prefix) + ": " + result.error().message, Color::Red);
        error = result.error();
        return false;
    }

    template<typename T> std::expected<T, err::Error> transform(const std::string& value)
    {

        try
        {
            const uint64_t temp = std::stoull(value);
            if (temp > std::numeric_limits<T>::max())
            {
                return std::unexpected(err::Error{err::TransformError::TransformFailed, "numeric_limits error max type >>>"});
            }
            return static_cast<T>(temp);
        }
        catch (const std::out_of_range& e)
        {
            return std::unexpected(err::Error{err::TransformError::TransformFailed, e.what()});
        }
        catch (const std::invalid_argument& e)
        {
            return std::unexpected(err::Error{err::TransformError::TransformFailed, e.what()});
        }
    }
}

#endif