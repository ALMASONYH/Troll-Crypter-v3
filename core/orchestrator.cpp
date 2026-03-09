// Troll Crypter v3 - Mason Group / Freemasonry
// Developed by ABOLHB

#include "core/orchestrator.h"
#include "crypto/engines.h"
#include "crypto/random.h"
#include "codegen/codegen.h"
#include <openssl/hmac.h>
#include <fstream>
#include <chrono>
#include <sstream>
#include <iomanip>

namespace troll {

static std::string bytes_to_hex(const Bytes& data) {
    std::ostringstream ss;
    for (auto b : data) ss << std::hex << std::setfill('0') << std::setw(2) << (int)b;
    return ss.str();
}

static Bytes string_to_bytes(const std::string& s) {
    return Bytes(s.begin(), s.end());
}

ProtectResult Orchestrator::protect(const std::string& filepath, ProgressCallback progress_cb) {
    ProtectResult result;
    auto t0 = std::chrono::steady_clock::now();

    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        result.error_msg = "Cannot open file: " + filepath;
        return result;
    }
    Bytes raw((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    result.original_size = raw.size();
    auto params = gen_params();
    Bytes data = raw;
    int total = cfg_.layers;

    for (int i = 0; i < total; i++) {
        if (progress_cb) progress_cb(i, total, "Layer " + std::to_string(i + 1));

        auto bundle = encrypt_layer(data, params[i]);

        try {
            verify_layer(data, bundle, params[i]);
        } catch (const std::exception& e) {
            result.error_msg = std::string("Self-test failed at layer ") + std::to_string(i+1) + ": " + e.what();
            return result;
        }

        std::string ns = (i == 0) ? "__main__" : (i == total - 1) ? "__outer__" : "__loader__";
        std::string code = LayerBuilder::build(bundle, params[i], raw.size(), ns, cfg_);
        data = string_to_bytes(code);
    }

    if (progress_cb) progress_cb(total, total, "Building stub...");

    std::string hex_body = bytes_to_hex(data);
    Bytes hex_bytes = string_to_bytes(hex_body);
    Bytes digest_raw = KDFEngine::sha256(hex_bytes);
    std::string digest = bytes_to_hex(digest_raw);

    Bytes hmac_key = Random::bytes(cfg_.hmac_key_size);
    unsigned int hmac_len = 0;
    Bytes hmac_tag(EVP_MAX_MD_SIZE);
    HMAC(EVP_sha512(), hmac_key.data(), hmac_key.size(),
         hex_bytes.data(), hex_bytes.size(), hmac_tag.data(), &hmac_len);
    hmac_tag.resize(hmac_len);

    std::string stub_code = StubBuilder::build(
        hex_body, digest, bytes_to_hex(hmac_key), bytes_to_hex(hmac_tag),
        raw.size(), cfg_
    );

    Bytes stub_bytes = string_to_bytes(stub_code);
    Bytes z = Compression::zlib_compress(stub_bytes, cfg_.compress_level);
    std::string b64 = Compression::base64_encode(z);

    std::string final_code = FragmentBuilder::build(b64, cfg_);

    std::string out_path = filepath;
    auto dot = out_path.rfind('.');
    if (dot != std::string::npos) out_path = out_path.substr(0, dot);
    out_path += cfg_.output_suffix + ".py";

    std::ofstream out(out_path, std::ios::binary);
    if (!out.is_open()) {
        result.error_msg = "Cannot write to: " + out_path;
        return result;
    }
    out.write(final_code.data(), final_code.size());
    out.close();

    auto t1 = std::chrono::steady_clock::now();
    result.success = true;
    result.output_path = out_path;
    result.output_size = final_code.size();
    result.elapsed_sec = std::chrono::duration<double>(t1 - t0).count();
    return result;
}

std::vector<LayerParams> Orchestrator::gen_params() {
    std::vector<LayerParams> out;
    for (int i = 0; i < cfg_.layers; i++) {
        LayerParams p;
        p.a = Random::bigint_string(cfg_.param_bits);
        p.x = Random::bigint_string(cfg_.key_bits);
        if (p.a == "0") p.a = "1";
        if (p.x == "0") p.x = "1";
        p.r = Random::range(cfg_.kdf_rounds_min, cfg_.kdf_rounds_max);
        p.aes_salt = Random::bytes(cfg_.salt_size);
        if (cfg_.use_chacha) p.chacha_salt = Random::bytes(cfg_.salt_size);
        out.push_back(p);
    }
    return out;
}

EncryptedBundle Orchestrator::encrypt_layer(const Bytes& data, const LayerParams& params) {
    EncryptedBundle bnd;

    bnd.aes_key = KDFEngine::derive_aes_key(params.a, params.x, params.r, params.aes_salt);
    Bytes compressed = Compression::zlib_compress(data, cfg_.compress_level);
    AESEngine::encrypt(compressed, bnd.aes_key, bnd.aes_nonce, bnd.aes_tag, bnd.aes_ct);
    bnd.aes_salt = params.aes_salt;

    if (cfg_.use_chacha) {
        bnd.chacha_key = KDFEngine::derive_chacha_key(params.a, params.x, params.r, params.chacha_salt);

        Bytes integrity;
        if (cfg_.use_integrity) {
            Bytes hash = KDFEngine::sha256(data);
            integrity = Compression::zlib_compress(hash, cfg_.compress_level);
        } else {
            integrity = Random::bytes(32);
        }

        ChaChaEngine::encrypt(integrity, bnd.chacha_key, bnd.chacha_nonce, bnd.chacha_tag, bnd.chacha_ct);
        bnd.chacha_salt = params.chacha_salt;
    }

    return bnd;
}

void Orchestrator::verify_layer(const Bytes& data, const EncryptedBundle& bnd, const LayerParams& params) {
    Bytes dec;
    if (!AESEngine::decrypt(bnd.aes_ct, bnd.aes_key, bnd.aes_nonce, bnd.aes_tag, dec))
        throw std::runtime_error("AES decrypt failed");

    Bytes plain = Compression::zlib_decompress(dec);
    if (plain != data) throw std::runtime_error("AES data mismatch");

    Bytes key2 = KDFEngine::derive_aes_key(params.a, params.x, params.r, params.aes_salt);
    if (key2 != bnd.aes_key) throw std::runtime_error("AES KDF non-deterministic");

    if (cfg_.use_chacha && !bnd.chacha_key.empty()) {
        Bytes cdec;
        if (!ChaChaEngine::decrypt(bnd.chacha_ct, bnd.chacha_key, bnd.chacha_nonce, bnd.chacha_tag, cdec))
            throw std::runtime_error("ChaCha decrypt failed");

        Bytes ck2 = KDFEngine::derive_chacha_key(params.a, params.x, params.r, params.chacha_salt);
        if (ck2 != bnd.chacha_key) throw std::runtime_error("ChaCha KDF non-deterministic");
    }
}

}
