#include "logs.hpp"

#include <chrono>
#include <filesystem>
#include <sstream>
#include <fstream>
#include "utils/files/paths.hpp"

namespace
{
    std::ofstream out_stream;

    std::stringstream string_stream_factory(const std::string_view type, const std::string_view message, const std::source_location location)
    {
        std::stringstream ss;
        ss << std::format(
            "<{:%Y-%m-%d %H:%M:%S}> ",
            std::chrono::zoned_time{std::chrono::current_zone(),std::chrono::system_clock::now()}
            ) << location.file_name() << ':' << location.line() << " in " << location.function_name() <<
                " ["<< type <<"] " << message << std::endl;
        return ss;
    }
}

std::expected<void, stx::err::Error> stx::log::init()
{
    std::filesystem::path path = paths::logs;

    out_stream.open(std::filesystem::path(path));
    return {};
}

void stx::log::shutdown()
{
    out_stream.close();
}

void stx::log::info(const std::string_view message, const std::source_location location)
{
    out_stream << string_stream_factory("INFO", message, location).str();
}

void stx::log::warn(const std::string_view message, const std::source_location location)
{
    out_stream << string_stream_factory("WARNING", message, location).str();
}

void stx::log::error(const std::string_view message, const std::source_location location)
{
    out_stream << string_stream_factory("ERROR", message, location).str();
}