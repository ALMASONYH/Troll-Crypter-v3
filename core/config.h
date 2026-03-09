#pragma once
#include <string>

namespace troll {

struct Config {
    int layers = 3;
    bool use_chacha = true;
    bool use_integrity = true;
    int kdf_rounds_min = 400;
    int kdf_rounds_max = 600;
    int key_bits = 128;
    int param_bits = 64;
    int junk_min = 2;
    int junk_max = 5;
    bool anti_debug = true;
    bool time_check = true;
    bool anti_vm = false;
    int num_parts_min = 10;
    int num_parts_max = 16;
    int marker_step_min = 7;
    int marker_step_max = 13;
    int compress_level = 9;
    int salt_size = 32;
    int hmac_key_size = 64;
    std::string output_suffix = "_protected";

    static Config Quick() {
        Config c;
        c.layers = 2;
        c.use_chacha = false;
        c.kdf_rounds_min = 100;
        c.kdf_rounds_max = 250;
        c.key_bits = 128;
        c.param_bits = 64;
        c.junk_min = 2;
        c.junk_max = 4;
        c.time_check = false;
        c.compress_level = 6;
        return c;
    }

    static Config Standard() {
        Config c;
        c.layers = 3;
        c.use_chacha = true;
        c.kdf_rounds_min = 400;
        c.kdf_rounds_max = 700;
        c.junk_min = 3;
        c.junk_max = 6;
        return c;
    }

    static Config Paranoid() {
        Config c;
        c.layers = 5;
        c.use_chacha = true;
        c.kdf_rounds_min = 800;
        c.kdf_rounds_max = 1400;
        c.key_bits = 256;
        c.param_bits = 128;
        c.junk_min = 5;
        c.junk_max = 10;
        c.anti_vm = true;
        return c;
    }
};

}
