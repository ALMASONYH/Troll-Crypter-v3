#pragma once
#include "core/types.h"
#include "core/config.h"
#include <string>

namespace troll {

class AntiDebugGen {
public:
    static std::string full();
    static std::string inline_check();
};

class AntiVMGen {
public:
    static std::string full();
};

class JunkGen {
public:
    static std::string lines(int count);
    static std::string mixed(int lo, int hi);
};

class KDFCodeGen {
public:
    static std::string aes(const std::string& pfx, const std::string& a_var,
                           const std::string& x_var, const std::string& r_var,
                           const std::string& salt_hex);
    static std::string chacha(const std::string& pfx, const std::string& a_var,
                              const std::string& x_var, const std::string& r_var,
                              const std::string& salt_hex);
};

class LayerBuilder {
public:
    static std::string build(const EncryptedBundle& bundle, const LayerParams& params,
                             size_t orig_size, const std::string& ns_name, const Config& cfg);
};

class StubBuilder {
public:
    static std::string build(const std::string& hex_body, const std::string& digest,
                             const std::string& hmac_key_hex, const std::string& hmac_tag_hex,
                             size_t size, const Config& cfg);
};

class FragmentBuilder {
public:
    static std::string build(const std::string& b64_data, const Config& cfg);
};

}
