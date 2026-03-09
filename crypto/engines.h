// Troll Crypter v3 - Mason Group / Freemasonry
// Developed by ABOLHB

#pragma once
#include "core/types.h"

namespace troll {

class AESEngine {
public:
    static bool encrypt(const Bytes& pt, const Bytes& key,
                        Bytes& nonce_out, Bytes& tag_out, Bytes& ct_out);
    static bool decrypt(const Bytes& ct, const Bytes& key,
                        const Bytes& nonce, const Bytes& tag, Bytes& pt_out);
};

class ChaChaEngine {
public:
    static bool encrypt(const Bytes& pt, const Bytes& key,
                        Bytes& nonce_out, Bytes& tag_out, Bytes& ct_out);
    static bool decrypt(const Bytes& ct, const Bytes& key,
                        const Bytes& nonce, const Bytes& tag, Bytes& pt_out);
};

class KDFEngine {
public:
    static Bytes sha512(const Bytes& data);
    static Bytes sha256(const Bytes& data);
    static Bytes hmac_sha512(const Bytes& key, const Bytes& data);
    static Bytes scrypt_derive(const Bytes& pw, const Bytes& salt,
                               int N = 16384, int r = 8, int p = 1, size_t klen = 32);
    static Bytes bigint_to_seed(const std::string& val);
    static Bytes kdf_stretch(const Bytes& material, int rounds);
    static Bytes aes_seed(const std::string& a, const std::string& x, int r);
    static Bytes chacha_seed(const std::string& a, const std::string& x, int r);
    static Bytes derive_aes_key(const std::string& a, const std::string& x, int r, const Bytes& salt);
    static Bytes derive_chacha_key(const std::string& a, const std::string& x, int r, const Bytes& salt);
};

class Compression {
public:
    static Bytes zlib_compress(const Bytes& data, int level = 9);
    static Bytes zlib_decompress(const Bytes& data);
    static std::string base64_encode(const Bytes& data);
    static Bytes base64_decode(const std::string& data);
    static std::string full_pack(const Bytes& data, int level = 9);
};

}
