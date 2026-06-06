#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <vector>
#include <string>
#include <unordered_map>
#include <cstdint>

struct HuffmanContainer {
    std::string algorithm;
    std::unordered_map<int, std::string> codes;
    std::vector<uint8_t> payload;
    int bit_length;
    size_t original_size;
};

class Huffman {
public:
    static HuffmanContainer encode(const std::vector<uint8_t>& data);
    static std::vector<uint8_t> decode(const HuffmanContainer& container);
    static int compressed_size_bits(const HuffmanContainer& container);
};

#endif
