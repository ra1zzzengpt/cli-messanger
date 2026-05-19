#include "crypto_sodium.h"

#include <stdexcept>
#include <string>
#include <sodium.h>
#include <sodium/crypto_secretbox.h>
// todo: to expected
#if defined(__linux__)

#include <fstream>

std::string CryptoSodium::machine_binding() {
    for (const char* path : {"/etc/machine-id", "/var/lib/dbus/machine-id"}) {
        std::ifstream f(path);
        if (!f) continue;
        std::string id;
        std::getline(f, id);
        if (id.size() >= 32) return id;
    }
    throw std::runtime_error("cannot read /etc/machine-id");
}

#elif defined(__APPLE__)

#include <array>
#include <cstdio>

std::string CryptoSodium::machine_binding() {
    const char* cmd =
        "ioreg -d2 -c IOPlatformExpertDevice "
        "| awk -F\\\" '/IOPlatformUUID/{print $4}'";

    std::array<char, 128> buf{};
    FILE* pipe = popen(cmd, "r");
    if (!pipe) throw std::runtime_error("popen failed");

    std::string id;
    if (fgets(buf.data(), buf.size(), pipe) != nullptr) {
        id = buf.data();
        while (!id.empty() && (id.back() == '\n' || id.back() == '\r')) {
            id.pop_back();
        }
    }
    pclose(pipe);

    if (id.size() < 32) throw std::runtime_error("cannot read IOPlatformUUID");
    return id;
}

#elif defined(_WIN32)

#include <windows.h>

std::string CryptoSodium::machine_binding() {
    HKEY key{};
    LSTATUS rc = RegOpenKeyExA(
        HKEY_LOCAL_MACHINE,
        "SOFTWARE\\Microsoft\\Cryptography",
        0,
        KEY_READ | KEY_WOW64_64KEY,
        &key);
    if (rc != ERROR_SUCCESS) throw std::runtime_error("RegOpenKeyEx failed");

    char  buffer[128] = {0};
    DWORD size        = sizeof(buffer);
    DWORD type        = 0;

    rc = RegQueryValueExA(key, "MachineGuid", nullptr, &type,
                          reinterpret_cast<BYTE*>(buffer), &size);
    RegCloseKey(key);

    if (rc != ERROR_SUCCESS || type != REG_SZ) {
        throw std::runtime_error("MachineGuid not found");
    }
    return std::string(buffer, size > 0 ? size - 1 : 0);
}

#else
#  error "Unsupported platform"
#endif

namespace
{
    constexpr auto APP_CERTIFICATE = "CLI-MESSANGER-CERTIFICATE-v0.4LBSDM127.2";
}

CryptoSodium::CryptoSodium()
{
    machine_id_ = machine_binding();
    machine_id_ += ":";
    machine_id_ += APP_CERTIFICATE;
    key_.resize(crypto_secretbox_KEYBYTES);
}

CryptoSodium::~CryptoSodium()
{
    if (!key_.empty()) {
        sodium_memzero(key_.data(), key_.size());
    }
    if (!machine_id_.empty()) {
        sodium_memzero(machine_id_.data(), machine_id_.size());
    }
}

void CryptoSodium::sodiumKeyGenerateBySalt(const std::vector<unsigned char>& salt) // todo: to expected
{
    if (!salt_.empty() && salt_ == salt)
    {
        return;
    }

    const int32_t rc = crypto_pwhash(
        key_.data(),
        key_.size(),
        machine_id_.c_str(),
        machine_id_.size(),
        salt.data(),
        crypto_pwhash_OPSLIMIT_INTERACTIVE,
        crypto_pwhash_MEMLIMIT_INTERACTIVE,
        crypto_pwhash_ALG_ARGON2ID13);

    if (rc != 0)
    {
        throw std::runtime_error("Argon2id failed (out of memory?)"); // todo: to expected error
    }
    salt_ = salt;
}

CryptoInfo CryptoSodium::encode(const std::string &plaintext)
{
    if (salt_.empty())
    {
        std::vector<unsigned char> fresh_salt(crypto_pwhash_SALTBYTES);
        randombytes_buf(fresh_salt.data(), fresh_salt.size());
        sodiumKeyGenerateBySalt(fresh_salt);
    }

    CryptoInfo crypto_info;

    crypto_info.salt = salt_;

    crypto_info.nonce.resize(crypto_secretbox_NONCEBYTES);
    randombytes_buf(crypto_info.nonce.data(), crypto_info.nonce.size());

    crypto_info.ciphertext.resize(plaintext.size() + crypto_secretbox_MACBYTES);

    crypto_secretbox_easy(
        crypto_info.ciphertext.data(),
        reinterpret_cast<const unsigned char*>(plaintext.data()),
        plaintext.size(),
        crypto_info.nonce.data(),
        key_.data());

    return crypto_info;
}

std::optional<std::string> CryptoSodium::decode(const CryptoInfo& crypto_info) // todo: to expected
{
    if (crypto_info.salt.size() != crypto_pwhash_SALTBYTES || crypto_info.nonce.size() != crypto_secretbox_NONCEBYTES
        || crypto_info.ciphertext.size() < crypto_secretbox_MACBYTES)
    {
        return std::nullopt;
    }
    try
    {
        sodiumKeyGenerateBySalt(crypto_info.salt);
    } catch (...)
    {
        return std::nullopt;
    }

    std::vector<unsigned char> plaintext(crypto_info.ciphertext.size() - crypto_secretbox_MACBYTES);

    const int32_t rc = crypto_secretbox_open_easy(
        plaintext.data(),
        crypto_info.ciphertext.data(),
        crypto_info.ciphertext.size(),
        crypto_info.nonce.data(),
        key_.data());

    if (rc != 0)
    {
        return std::nullopt;
    }

    return std::string(reinterpret_cast<char*>(plaintext.data()), plaintext.size());
}