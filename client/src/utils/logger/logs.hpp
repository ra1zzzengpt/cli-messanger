#ifndef LOGS_HPP
#define LOGS_HPP
#include <expected>
#include <source_location>

#include <utils/error/app_error.hpp>

namespace stx::log
{
    std::expected<void, err::Error> init();
    void shutdown();

    void info(std::string_view message, std::source_location location = std::source_location::current());
    void warn(std::string_view message, std::source_location location = std::source_location::current());
    void error(std::string_view message, std::source_location location = std::source_location::current());
}
#endif