#pragma once

namespace stx::err
{
    enum class FileError
    {
        OpenFileFailed,
        NotFound
    };

    enum class ConfigError
    {
        CreateDirectoryFailed,
        OpenFileFailed,
        EncryptionFailed,
        DecryptionFailed,
        ParseFailed,
        IncorrectConfiguration,
    };

    enum class CryptoError
    {
        ReadIdFailed,
        OutOfMemory,
        BrokenFile,
        SecretboxOpenFailed,
    };

    enum class NetworkError
    {
        CurlInitFailed   = 1,
        ConnectionFailed = 2,
        Timeout          = 3,
        JsonParseError   = 4,
        InvalidResponse  = 5,
        BadRequest       = 6,
        Unauthorized     = 7,
        NotFound         = 8,
        Conflict         = 9,
        ServerError      = 10,
        UnexpectedStatus = 11,
    };

    enum class CommandError
    {
        UnknownCommand,
    };

    enum class Base64Error
    {
        SodiumBase64Error,
    };

    enum class JsonError {
        ParsingFailed,
    };
}