#pragma once
#include "core/types.h"
#include "core/config.h"
#include <string>

namespace troll {

class Orchestrator {
public:
    explicit Orchestrator(const Config& cfg) : cfg_(cfg) {}

    ProtectResult protect(const std::string& filepath,
                          ProgressCallback progress = nullptr);

private:
    Config cfg_;

    std::vector<LayerParams> gen_params();
    EncryptedBundle encrypt_layer(const Bytes& data, const LayerParams& params);
    void verify_layer(const Bytes& data, const EncryptedBundle& bnd, const LayerParams& params);
};

}
