// Troll Crypter v3 - Mason Group / Freemasonry
// Developed by ABOLHB

#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include <functional>

namespace troll {

using Bytes = std::vector<uint8_t>;

struct EncryptedBundle {
    Bytes aes_ct;
    Bytes aes_nonce;
    Bytes aes_tag;
    Bytes aes_salt;
    Bytes aes_key;

    Bytes chacha_ct;
    Bytes chacha_nonce;
    Bytes chacha_tag;
    Bytes chacha_salt;
    Bytes chacha_key;
};

struct LayerParams {
    std::string a;
    std::string x;
    int r = 0;
    Bytes aes_salt;
    Bytes chacha_salt;
};

enum class ProtectMode {
    Quick,
    Standard,
    Paranoid,
    Custom
};

struct ProtectResult {
    bool success = false;
    std::string output_path;
    std::string error_msg;
    size_t original_size = 0;
    size_t output_size = 0;
    double elapsed_sec = 0.0;
};

using ProgressCallback = std::function<void(int current, int total, const std::string& msg)>;

}
