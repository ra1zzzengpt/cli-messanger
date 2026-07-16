#include <utils/logger/logs.hpp>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <format>
#include <sstream>
#include <system_error>
#include <utils/files/paths.hpp>

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
    const std::filesystem::path path = paths::logs;

    std::error_code error_code;
    std::filesystem::create_directories(path.parent_path(), error_code);
    if (error_code)
    {
        return std::unexpected(err::Error{
            err::FileError::OpenFileFailed,
            "failed to create log directory '" + path.parent_path().string() + "': " + error_code.message()
        });
    }

    out_stream.open(path, std::ios::out | std::ios::trunc);
    if (!out_stream.is_open())
    {
        return std::unexpected(err::Error{
            err::FileError::OpenFileFailed,
            "failed to open log file '" + path.string() + "'"
        });
    }
    return {};
}

void stx::log::shutdown()
{
    out_stream.flush();
    out_stream.close();
}

void stx::log::info(const std::string_view message, const std::source_location location)
{
    out_stream << string_stream_factory("INFO", message, location).str() << std::flush;
}

void stx::log::warn(const std::string_view message, const std::source_location location)
{
    out_stream << string_stream_factory("WARNING", message, location).str() << std::flush;
}

void stx::log::error(const std::string_view message, const std::source_location location)
{
    out_stream << string_stream_factory("ERROR", message, location).str() << std::flush;
}
