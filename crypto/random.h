#pragma once
#include "core/types.h"
#include <random>
#include <string>

namespace troll {

class Random {
public:
    static Bytes bytes(size_t count);
    static int range(int lo, int hi);
    static uint64_t bits(int num_bits);
    static std::string name(int min_len = 6, int max_len = 11);
    static std::string hex_string(size_t byte_count = 16);
    static std::string bigint_string(int num_bits);

private:
    static std::mt19937_64& engine();
};

}
