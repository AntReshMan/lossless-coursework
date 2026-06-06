#include <iostream>
#include <cassert>
#include <string>
#include <vector>

#include "huffman.h"
#include "lzw.h"

void test_roundtrip_huffman() {
    std::string input_str = "пример пример пример: сжатие без потерь";
    std::vector<uint8_t> data(input_str.begin(), input_str.end());
    
    HuffmanContainer packed = Huffman::encode(data);
    std::vector<uint8_t> decoded = Huffman::decode(packed);
    
    assert(data == decoded);
    std::cout << "Huffman test passed\n";
}

void test_roundtrip_lzw() {
    std::string input_str = "TOBEORNOTTOBEORTOBEORNOT";
    std::vector<uint8_t> data(input_str.begin(), input_str.end());
    
    LZWContainer packed = LZW::encode(data);
    std::vector<uint8_t> decoded = LZW::decode(packed);
    
    assert(data == decoded);
    std::cout << "LZW test passed\n";
}

int main() {
    test_roundtrip_huffman();
    test_roundtrip_lzw();
    std::cout << "All tests passed!\n";
    return 0;
}
