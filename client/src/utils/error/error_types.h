#pragma once

namespace utils::errors
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
        ParseFailed
    };

    enum class CryptoError
    {
        ReadIdFailed,
        OutOfMemory,
        BrokenFile,
        SecretboxOpenFailed,
    };

    enum class NetError
    {
        CurlInitFailed   = 1,  // curl_easy_init() вернул nullptr — редкая системная ошибка
        ConnectionFailed = 2,  // соединение не установлено: сервер недоступен, нет сети
        Timeout          = 3,  // запрос превысил таймаут (5 секунд)
        JsonParseError   = 4,  // тело ответа не является валидным JSON
        InvalidResponse  = 5,  // JSON валиден, но ожидаемые поля отсутствуют
        BadRequest       = 6,  // HTTP 400 — невалидные параметры (пустой nick, короткий пароль и т.д.)
        Unauthorized     = 7,  // HTTP 401 — неверный пароль
        NotFound         = 8,  // HTTP 404 — пользователь или ресурс не найден
        Conflict         = 9,  // HTTP 409 — ресурс уже существует (например, id занят)
        ServerError      = 10, // HTTP 5xx — внутренняя ошибка сервера
        UnexpectedStatus = 11, // любой другой HTTP-статус
    };

    enum class CommandError
    {
        UnknownCommand,
    };

    enum class Base64Error
    {
        SodiumBase64Error,
    };
}