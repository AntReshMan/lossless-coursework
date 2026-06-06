#include "huffman.h"
#include <queue>
#include <memory>
#include <bitset>
#include <stdexcept>
#include <algorithm>

struct Node {
    int freq;
    int symbol;
    std::shared_ptr<Node> left;
    std::shared_ptr<Node> right;

    Node(int freq, int symbol = -1, 
         std::shared_ptr<Node> left = nullptr, 
         std::shared_ptr<Node> right = nullptr)
        : freq(freq), symbol(symbol), left(left), right(right) {}

    bool is_leaf() const {
        return symbol != -1;
    }
};

struct CompareNode {
    bool operator()(const std::shared_ptr<Node>& a, const std::shared_ptr<Node>& b) const {
        return a->freq > b->freq;
    }
};

static std::shared_ptr<Node> build_tree(const std::vector<uint8_t>& data) {
    if (data.empty()) return nullptr;
    
    std::vector<int> frequencies(256, 0);
    for (uint8_t byte : data) {
        frequencies[byte]++;
    }
    
    std::priority_queue<std::shared_ptr<Node>, 
                       std::vector<std::shared_ptr<Node>>, 
                       CompareNode> heap;
    
    for (int i = 0; i < 256; i++) {
        if (frequencies[i] > 0) {
            heap.push(std::make_shared<Node>(frequencies[i], i));
        }
    }
    
    if (heap.empty()) return nullptr;
    
    if (heap.size() == 1) {
        return heap.top();
    }
    
    while (heap.size() > 1) {
        auto left = heap.top(); heap.pop();
        auto right = heap.top(); heap.pop();
        auto parent = std::make_shared<Node>(left->freq + right->freq, -1, left, right);
        heap.push(parent);
    }
    
    return heap.top();
}

static void walk_tree(std::shared_ptr<Node> node, const std::string& prefix, 
                     std::vector<std::string>& codes) {
    if (!node) return;
    
    if (node->is_leaf()) {
        codes[node->symbol] = prefix.empty() ? "0" : prefix;
        return;
    }
    
    if (node->left) walk_tree(node->left, prefix + "0", codes);
    if (node->right) walk_tree(node->right, prefix + "1", codes);
}

static std::pair<std::vector<uint8_t>, int> pack_bits(const std::string& bit_string) {
    int bit_length = bit_string.length();
    int padding = (8 - (bit_length % 8)) % 8;
    
    std::string padded_bits = bit_string + std::string(padding, '0');
    std::vector<uint8_t> out;
    out.reserve(padded_bits.length() / 8);
    
    for (size_t i = 0; i < padded_bits.length(); i += 8) {
        uint8_t byte = 0;
        for (size_t j = 0; j < 8 && i + j < padded_bits.length(); j++) {
            if (padded_bits[i + j] == '1') {
                byte |= (1 << (7 - j));
            }
        }
        out.push_back(byte);
    }
    
    return {out, bit_length};
}

static std::string unpack_bits(const std::vector<uint8_t>& payload, int bit_length) {
    std::string result;
    result.reserve(bit_length);
    
    for (size_t i = 0; i < payload.size() && result.length() < (size_t)bit_length; i++) {
        uint8_t byte = payload[i];
        for (int j = 7; j >= 0 && result.length() < (size_t)bit_length; j--) {
            result += ((byte >> j) & 1) ? '1' : '0';
        }
    }
    
    return result;
}

HuffmanContainer Huffman::encode(const std::vector<uint8_t>& data) {
    HuffmanContainer container;
    container.algorithm = "huffman";
    container.original_size = data.size();
    
    if (data.empty()) {
        container.codes = {};
        container.payload = {};
        container.bit_length = 0;
        return container;
    }
    
    auto root = build_tree(data);
    if (!root) {
        container.codes = {};
        container.payload = {};
        container.bit_length = 0;
        return container;
    }
    
    std::vector<std::string> codes(256);
    walk_tree(root, "", codes);
    
    // Сохраняем только ненулевые коды
    for (int i = 0; i < 256; i++) {
        if (!codes[i].empty()) {
            container.codes[i] = codes[i];
        }
    }
    
    std::string bit_string;
    bit_string.reserve(data.size() * 8);
    for (uint8_t byte : data) {
        bit_string += codes[byte];
    }
    
    auto [payload, bit_length] = pack_bits(bit_string);
    container.payload = payload;
    container.bit_length = bit_length;
    
    return container;
}

std::vector<uint8_t> Huffman::decode(const HuffmanContainer& container) {
    if (container.codes.empty() || container.payload.empty()) {
        return {};
    }
    
    // Построение обратного словаря
    std::unordered_map<std::string, int> reverse_codes;
    for (const auto& [symbol, code] : container.codes) {
        reverse_codes[code] = symbol;
    }
    
    std::string bits = unpack_bits(container.payload, container.bit_length);
    
    std::vector<uint8_t> result;
    result.reserve(container.original_size);
    std::string current;
    
    for (char bit : bits) {
        current += bit;
        auto it = reverse_codes.find(current);
        if (it != reverse_codes.end()) {
            result.push_back(static_cast<uint8_t>(it->second));
            current.clear();
        }
    }
    
    return result;
}

int Huffman::compressed_size_bits(const HuffmanContainer& container) {
    return container.bit_length;
}
