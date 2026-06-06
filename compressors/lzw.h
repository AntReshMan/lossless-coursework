#ifndef LZW_H
#define LZW_H

#include <vector>
#include <string>
#include <cstdint>

struct LZWContainer {
    std::string algorithm;
    std::vector<int> codes;
    size_t original_size;
    int code_width;
};

class LZW {
public:
    static LZWContainer encode(const std::vector<uint8_t>& data, int max_code = 4095);
    static std::vector<uint8_t> decode(const LZWContainer& container, int max_code = 4095);
    static int compressed_size_bits(const LZWContainer& container);
};

#endif
