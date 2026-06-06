#include "lzw.h"
#include <unordered_map>
#include <string>
#include <vector>
#include <cstdint>
#include <stdexcept>

// Вспомогательная структура для хеширования vector<uint8_t>
struct VectorHash {
    std::size_t operator()(const std::vector<uint8_t>& v) const {
        std::size_t hash = 0;
        for (uint8_t byte : v) {
            hash = hash * 31 + byte;
        }
        return hash;
    }
};

// Вспомогательная структура для сравнения vector<uint8_t>
struct VectorEqual {
    bool operator()(const std::vector<uint8_t>& a, const std::vector<uint8_t>& b) const {
        return a == b;
    }
};

LZWContainer LZW::encode(const std::vector<uint8_t>& data, int max_code) {
    LZWContainer result;
    result.algorithm = "lzw";
    result.code_width = 12;
    result.original_size = data.size();
    
    if (data.empty()) {
        result.codes = {};
        return result;
    }
    
    // Используем unordered_map с пользовательскими хеш-функциями
    std::unordered_map<std::vector<uint8_t>, int, VectorHash, VectorEqual> dictionary;
    
    // Инициализация словаря всеми возможными байтами (0-255)
    for (int i = 0; i < 256; i++) {
        dictionary[{static_cast<uint8_t>(i)}] = i;
    }
    
    int next_code = 256;
    std::vector<uint8_t> w = {data[0]};
    std::vector<int> codes;
    
    for (size_t i = 1; i < data.size(); i++) {
        uint8_t byte = data[i];
        std::vector<uint8_t> wk = w;
        wk.push_back(byte);
        
        if (dictionary.find(wk) != dictionary.end()) {
            w = wk;
        } else {
            codes.push_back(dictionary[w]);
            if (next_code <= max_code) {
                dictionary[wk] = next_code;
                next_code++;
            }
            w = {byte};
        }
    }
    
    codes.push_back(dictionary[w]);
    result.codes = codes;
    return result;
}

std::vector<uint8_t> LZW::decode(const LZWContainer& container, int max_code) {
    const std::vector<int>& codes = container.codes;
    
    if (codes.empty()) {
        return {};
    }
    
    // Для декодирования используем обычную map (не нужно хеширование)
    std::unordered_map<int, std::vector<uint8_t>> dictionary;
    for (int i = 0; i < 256; i++) {
        dictionary[i] = {static_cast<uint8_t>(i)};
    }
    
    int next_code = 256;
    std::vector<uint8_t> w = dictionary[codes[0]];
    std::vector<uint8_t> out = w;
    
    for (size_t i = 1; i < codes.size(); i++) {
        int code = codes[i];
        std::vector<uint8_t> entry;
        
        if (dictionary.find(code) != dictionary.end()) {
            entry = dictionary[code];
        } else if (code == next_code) {
            entry = w;
            if (!w.empty()) {
                entry.push_back(w[0]);
            } else {
                throw std::runtime_error("Ошибка декодирования LZW");
            }
        } else {
            throw std::runtime_error("Некорректный LZW код: " + std::to_string(code));
        }
        
        out.insert(out.end(), entry.begin(), entry.end());
        
        if (next_code <= max_code) {
            std::vector<uint8_t> new_entry = w;
            if (!entry.empty()) {
                new_entry.push_back(entry[0]);
                dictionary[next_code] = new_entry;
                next_code++;
            }
        }
        
        w = entry;
    }
    
    return out;
}

int LZW::compressed_size_bits(const LZWContainer& container) {
    return container.codes.size() * container.code_width;
}
