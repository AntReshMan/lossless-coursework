#include <vector>
#include <string>
#include <unordered_map>
#include <queue>
#include <memory>
#include <optional>
#include <stdexcept>
#include <cstdint>
#include <bitset>

// Структура для узла дерева Хаффмана
struct Node {
    int freq;
    std::optional<int> symbol;
    std::shared_ptr<Node> left;
    std::shared_ptr<Node> right;

    Node(int freq, std::optional<int> symbol = std::nullopt, 
         std::shared_ptr<Node> left = nullptr, 
         std::shared_ptr<Node> right = nullptr)
        : freq(freq), symbol(symbol), left(left), right(right) {}

    bool is_leaf() const {
        return symbol.has_value();
    }
};

// Структура для контейнера с результатами сжатия
struct HuffmanContainer {
    std::string algorithm;
    std::unordered_map<int, std::string> codes;  // символ -> битовая строка
    std::vector<uint8_t> payload;
    int bit_length;
    size_t original_size;
};

// Компаратор для min-heap
struct CompareNode {
    bool operator()(const std::shared_ptr<Node>& a, const std::shared_ptr<Node>& b) const {
        return a->freq > b->freq;
    }
};

class Huffman {
private:
    static const int BYTE_SIZE = 256;

    static std::shared_ptr<Node> build_tree(const std::vector<uint8_t>& data) {
        if (data.empty()) {
            return nullptr;
        }

        // Подсчет частот
        std::unordered_map<int, int> frequencies;
        for (uint8_t byte : data) {
            frequencies[byte]++;
        }

        // Min-heap для узлов
        auto cmp = [](const std::shared_ptr<Node>& a, const std::shared_ptr<Node>& b) {
            return a->freq > b->freq;
        };
        std::priority_queue<std::shared_ptr<Node>, 
                           std::vector<std::shared_ptr<Node>>, 
                           decltype(cmp)> heap(cmp);

        // Создание листьев
        for (const auto& [symbol, freq] : frequencies) {
            heap.push(std::make_shared<Node>(freq, symbol));
        }

        // Если только один уникальный символ
        if (heap.size() == 1) {
            return heap.top();
        }

        // Построение дерева
        while (heap.size() > 1) {
            auto left = heap.top(); heap.pop();
            auto right = heap.top(); heap.pop();
            auto parent = std::make_shared<Node>(left->freq + right->freq, 
                                                std::nullopt, left, right);
            heap.push(parent);
        }

        return heap.top();
    }

    static void walk_tree(std::shared_ptr<Node> node, const std::string& prefix, 
                         std::unordered_map<int, std::string>& codes) {
        if (!node) return;

        if (node->is_leaf()) {
            // Для единственного символа нужен хотя бы один бит кода
            codes[node->symbol.value()] = prefix.empty() ? "0" : prefix;
            return;
        }

        if (node->left) {
            walk_tree(node->left, prefix + "0", codes);
        }
        if (node->right) {
            walk_tree(node->right, prefix + "1", codes);
        }
    }

    static std::pair<std::vector<uint8_t>, int> pack_bits(const std::string& bit_string) {
        int bit_length = bit_string.length();
        int padding = (8 - (bit_length % 8)) % 8;
        
        std::string padded_bits = bit_string + std::string(padding, '0');
        std::vector<uint8_t> out;
        
        for (size_t i = 0; i < padded_bits.length(); i += 8) {
            std::string byte_str = padded_bits.substr(i, 8);
            uint8_t byte = static_cast<uint8_t>(std::stoi(byte_str, nullptr, 2));
            out.push_back(byte);
        }
        
        return {out, bit_length};
    }

    static std::string unpack_bits(const std::vector<uint8_t>& payload, int bit_length) {
        std::string result;
        for (uint8_t byte : payload) {
            result += std::bitset<8>(byte).to_string();
        }
        return result.substr(0, bit_length);
    }

public:
    static HuffmanContainer encode(const std::vector<uint8_t>& data) {
        HuffmanContainer container;
        container.algorithm = "huffman";
        container.original_size = data.size();

        auto root = build_tree(data);
        if (!root) {
            container.codes = {};
            container.payload = {};
            container.bit_length = 0;
            return container;
        }

        // Построение кодов
        std::unordered_map<int, std::string> codes;
        walk_tree(root, "", codes);
        container.codes = codes;

        // Формирование битовой строки
        std::string bit_string;
        for (uint8_t byte : data) {
            bit_string += codes[byte];
        }

        // Упаковка битов
        auto [payload, bit_length] = pack_bits(bit_string);
        container.payload = payload;
        container.bit_length = bit_length;

        return container;
    }

    static std::vector<uint8_t> decode(const HuffmanContainer& container) {
        // Построение обратного словаря
        std::unordered_map<std::string, int> reverse_codes;
        for (const auto& [symbol, code] : container.codes) {
            reverse_codes[code] = symbol;
        }

        // Распаковка битов
        std::string bits = unpack_bits(container.payload, container.bit_length);

        // Декодирование
        std::vector<uint8_t> result;
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
    int compressed_size_bits(const HuffmanContainer &container);
};

// Вспомогательные функции для удобства использования
HuffmanContainer encode_bytes(const uint8_t* data, size_t size) {
    std::vector<uint8_t> vec(data, data + size);
    return Huffman::encode(vec);
}

std::vector<uint8_t> decode_container(const HuffmanContainer& container) {
    return Huffman::decode(container);
}

int Huffman::compressed_size_bits(const HuffmanContainer& container) {
    return container.bit_length;
}
