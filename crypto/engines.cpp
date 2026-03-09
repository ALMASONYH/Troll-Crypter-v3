// Troll Crypter v3 - Mason Group / Freemasonry
// Developed by ABOLHB

#include "crypto/engines.h"
#include "crypto/random.h"
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <openssl/hmac.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <zlib.h>
#include <cstring>
#include <sstream>
#include <algorithm>
#include <stdexcept>

namespace troll {

bool AESEngine::encrypt(const Bytes& pt, const Bytes& key,
                        Bytes& nonce_out, Bytes& tag_out, Bytes& ct_out) {
    nonce_out = Random::bytes(12);
    ct_out.resize(pt.size());
    tag_out.resize(16);
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) return false;
    int len = 0;
    bool ok = true;
    ok = ok && EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), nullptr, nullptr, nullptr);
    ok = ok && EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, 12, nullptr);
    ok = ok && EVP_EncryptInit_ex(ctx, nullptr, nullptr, key.data(), nonce_out.data());
    ok = ok && EVP_EncryptUpdate(ctx, ct_out.data(), &len, pt.data(), (int)pt.size());
    ok = ok && EVP_EncryptFinal_ex(ctx, ct_out.data() + len, &len);
    ok = ok && EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, 16, tag_out.data());
    EVP_CIPHER_CTX_free(ctx);
    return ok;
}

bool AESEngine::decrypt(const Bytes& ct, const Bytes& key,
                        const Bytes& nonce, const Bytes& tag, Bytes& pt_out) {
    pt_out.resize(ct.size());
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) return false;
    int len = 0;
    bool ok = true;
    ok = ok && EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), nullptr, nullptr, nullptr);
    ok = ok && EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, 12, nullptr);
    ok = ok && EVP_DecryptInit_ex(ctx, nullptr, nullptr, key.data(), nonce.data());
    ok = ok && EVP_DecryptUpdate(ctx, pt_out.data(), &len, ct.data(), (int)ct.size());
    ok = ok && EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, 16, (void*)tag.data());
    ok = ok && EVP_DecryptFinal_ex(ctx, pt_out.data() + len, &len);
    EVP_CIPHER_CTX_free(ctx);
    return ok;
}

bool ChaChaEngine::encrypt(const Bytes& pt, const Bytes& key,
                           Bytes& nonce_out, Bytes& tag_out, Bytes& ct_out) {
    nonce_out = Random::bytes(12);
    ct_out.resize(pt.size());
    tag_out.resize(16);
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) return false;
    int len = 0;
    bool ok = true;
    ok = ok && EVP_EncryptInit_ex(ctx, EVP_chacha20_poly1305(), nullptr, nullptr, nullptr);
    ok = ok && EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_AEAD_SET_IVLEN, 12, nullptr);
    ok = ok && EVP_EncryptInit_ex(ctx, nullptr, nullptr, key.data(), nonce_out.data());
    ok = ok && EVP_EncryptUpdate(ctx, ct_out.data(), &len, pt.data(), (int)pt.size());
    ok = ok && EVP_EncryptFinal_ex(ctx, ct_out.data() + len, &len);
    ok = ok && EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_AEAD_GET_TAG, 16, tag_out.data());
    EVP_CIPHER_CTX_free(ctx);
    return ok;
}

bool ChaChaEngine::decrypt(const Bytes& ct, const Bytes& key,
                           const Bytes& nonce, const Bytes& tag, Bytes& pt_out) {
    pt_out.resize(ct.size());
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) return false;
    int len = 0;
    bool ok = true;
    ok = ok && EVP_DecryptInit_ex(ctx, EVP_chacha20_poly1305(), nullptr, nullptr, nullptr);
    ok = ok && EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_AEAD_SET_IVLEN, 12, nullptr);
    ok = ok && EVP_DecryptInit_ex(ctx, nullptr, nullptr, key.data(), nonce.data());
    ok = ok && EVP_DecryptUpdate(ctx, pt_out.data(), &len, ct.data(), (int)ct.size());
    ok = ok && EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_AEAD_SET_TAG, 16, (void*)tag.data());
    ok = ok && EVP_DecryptFinal_ex(ctx, pt_out.data() + len, &len);
    EVP_CIPHER_CTX_free(ctx);
    return ok;
}

Bytes KDFEngine::sha512(const Bytes& data) {
    Bytes out(SHA512_DIGEST_LENGTH);
    SHA512(data.data(), data.size(), out.data());
    return out;
}

Bytes KDFEngine::sha256(const Bytes& data) {
    Bytes out(SHA256_DIGEST_LENGTH);
    SHA256(data.data(), data.size(), out.data());
    return out;
}

Bytes KDFEngine::hmac_sha512(const Bytes& key, const Bytes& data) {
    unsigned int len = 0;
    Bytes out(EVP_MAX_MD_SIZE);
    HMAC(EVP_sha512(), key.data(), (int)key.size(),
         data.data(), data.size(), out.data(), &len);
    out.resize(len);
    return out;
}

Bytes KDFEngine::scrypt_derive(const Bytes& pw, const Bytes& salt,
                                int N, int r, int p, size_t klen) {
    Bytes out(klen);
    int ret = EVP_PBE_scrypt(
        (const char*)pw.data(), pw.size(),
        salt.data(), salt.size(),
        (uint64_t)N, (uint64_t)r, (uint64_t)p,
        0,
        out.data(), klen
    );
    if (ret != 1) throw std::runtime_error("scrypt failed");
    return out;
}

static Bytes decimal_to_bytes(const std::string& dec) {
    if (dec.empty() || dec == "0") return {0x01};
    std::vector<uint8_t> num;
    for (char c : dec) {
        uint32_t carry = c - '0';
        for (int i = (int)num.size() - 1; i >= 0; i--) {
            uint32_t val = (uint32_t)num[i] * 10 + carry;
            num[i] = val & 0xFF;
            carry = val >> 8;
        }
        while (carry) {
            num.insert(num.begin(), carry & 0xFF);
            carry >>= 8;
        }
        if (num.empty()) num.push_back(0);
    }
    while (num.size() > 1 && num.front() == 0) num.erase(num.begin());
    if (num.empty()) num.push_back(0x01);
    return Bytes(num.begin(), num.end());
}

static std::string mul_dec(const std::string& a, const std::string& b) {
    if (a == "0" || b == "0") return "0";
    int n = (int)a.size(), m = (int)b.size();
    std::vector<int> r(n + m, 0);
    for (int i = n - 1; i >= 0; i--)
        for (int j = m - 1; j >= 0; j--) {
            int mul = (a[i] - '0') * (b[j] - '0');
            int s = mul + r[i + j + 1];
            r[i + j + 1] = s % 10;
            r[i + j] += s / 10;
        }
    std::string out;
    for (int x : r) if (!(out.empty() && x == 0)) out += (char)('0' + x);
    return out.empty() ? "0" : out;
}

static std::string xor_dec(const std::string& a, const std::string& b) {
    Bytes ba = decimal_to_bytes(a);
    Bytes bb = decimal_to_bytes(b);
    size_t mx = std::max(ba.size(), bb.size());
    while (ba.size() < mx) ba.insert(ba.begin(), 0);
    while (bb.size() < mx) bb.insert(bb.begin(), 0);
    Bytes res(mx);
    for (size_t i = 0; i < mx; i++) res[i] = ba[i] ^ bb[i];

    std::string dec;
    while (!res.empty()) {
        uint32_t rem = 0;
        std::vector<uint8_t> q;
        for (auto byte : res) {
            uint32_t d = rem * 256 + byte;
            uint8_t qv = (uint8_t)(d / 10);
            rem = d % 10;
            if (!q.empty() || qv > 0) q.push_back(qv);
        }
        dec += (char)('0' + rem);
        res = Bytes(q.begin(), q.end());
    }
    std::reverse(dec.begin(), dec.end());
    return dec.empty() ? "0" : dec;
}

static std::string maxs(const std::string& a) {
    return (a.empty() || a == "0") ? "1" : a;
}

Bytes KDFEngine::bigint_to_seed(const std::string& val) {
    std::string v = maxs(val);
    Bytes raw = decimal_to_bytes(v);
    return sha512(raw);
}

Bytes KDFEngine::kdf_stretch(const Bytes& material, int rounds) {
    Bytes buf = material;
    for (int i = 0; i < rounds; i++) {
        Bytes input = buf;
        uint8_t ib[4] = {
            (uint8_t)((i >> 24) & 0xFF), (uint8_t)((i >> 16) & 0xFF),
            (uint8_t)((i >> 8) & 0xFF), (uint8_t)(i & 0xFF)
        };
        input.insert(input.end(), ib, ib + 4);
        buf = sha512(input);
        buf = hmac_sha512(material, buf);
    }
    return buf;
}

Bytes KDFEngine::aes_seed(const std::string& a, const std::string& x, int r) {
    std::string v1 = maxs(mul_dec(a, x));
    std::string v2 = maxs(xor_dec(x, std::to_string(r)));
    std::string v3 = maxs(mul_dec(std::to_string(r), a));
    Bytes cat;
    auto s1 = bigint_to_seed(v1), s2 = bigint_to_seed(v2), s3 = bigint_to_seed(v3);
    cat.insert(cat.end(), s1.begin(), s1.end());
    cat.insert(cat.end(), s2.begin(), s2.end());
    cat.insert(cat.end(), s3.begin(), s3.end());
    Bytes mat = sha512(cat);
    return kdf_stretch(mat, r);
}

Bytes KDFEngine::chacha_seed(const std::string& a, const std::string& x, int r) {
    std::string v1 = maxs(xor_dec(a, x));
    std::string v2 = maxs(mul_dec(x, std::to_string(r)));
    Bytes cat;
    auto s1 = bigint_to_seed(v1), s2 = bigint_to_seed(v2);
    cat.insert(cat.end(), s1.begin(), s1.end());
    cat.insert(cat.end(), s2.begin(), s2.end());
    Bytes mat = sha512(cat);
    return kdf_stretch(mat, r);
}

Bytes KDFEngine::derive_aes_key(const std::string& a, const std::string& x, int r, const Bytes& salt) {
    return scrypt_derive(aes_seed(a, x, r), salt);
}

Bytes KDFEngine::derive_chacha_key(const std::string& a, const std::string& x, int r, const Bytes& salt) {
    return scrypt_derive(chacha_seed(a, x, r), salt);
}

Bytes Compression::zlib_compress(const Bytes& data, int level) {
    uLong bound = compressBound((uLong)data.size());
    Bytes out(bound);
    uLong out_len = bound;
    compress2(out.data(), &out_len, data.data(), (uLong)data.size(), level);
    out.resize(out_len);
    return out;
}

Bytes Compression::zlib_decompress(const Bytes& data) {
    Bytes out(data.size() * 10);
    uLong out_len = (uLong)out.size();
    while (true) {
        int ret = uncompress(out.data(), &out_len, data.data(), (uLong)data.size());
        if (ret == Z_OK) break;
        if (ret == Z_BUF_ERROR) {
            out.resize(out.size() * 2);
            out_len = (uLong)out.size();
        } else throw std::runtime_error("zlib decompress failed");
    }
    out.resize(out_len);
    return out;
}

std::string Compression::base64_encode(const Bytes& data) {
    BIO* b64 = BIO_new(BIO_f_base64());
    BIO* mem = BIO_new(BIO_s_mem());
    b64 = BIO_push(b64, mem);
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    BIO_write(b64, data.data(), (int)data.size());
    BIO_flush(b64);
    BUF_MEM* bptr = nullptr;
    BIO_get_mem_ptr(b64, &bptr);
    std::string result(bptr->data, bptr->length);
    BIO_free_all(b64);
    return result;
}

Bytes Compression::base64_decode(const std::string& data) {
    BIO* b64 = BIO_new(BIO_f_base64());
    BIO* mem = BIO_new_mem_buf(data.data(), (int)data.size());
    mem = BIO_push(b64, mem);
    BIO_set_flags(mem, BIO_FLAGS_BASE64_NO_NL);
    Bytes out(data.size());
    int len = BIO_read(mem, out.data(), (int)out.size());
    BIO_free_all(mem);
    if (len > 0) out.resize(len);
    else out.clear();
    return out;
}

std::string Compression::full_pack(const Bytes& data, int level) {
    Bytes z = zlib_compress(data, level);
    return base64_encode(z);
}

}
