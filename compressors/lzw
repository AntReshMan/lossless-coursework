#include <vector>
#include <string>
#include <unordered_map>
#include <stdexcept>
#include <cstdint>

struct LZWContainer {
    std::string algorithm;
    std::vector<int> codes;
    size_t original_size;
    int code_width;
};

class LZW {
private:
    static const int DEFAULT_MAX_CODE = 4095;
    static const int DEFAULT_CODE_WIDTH = 12;

public:
    static LZWContainer encode(const std::vector<uint8_t>& data, int max_code = DEFAULT_MAX_CODE) {
        LZWContainer result;
        result.algorithm = "lzw";
        result.code_width = DEFAULT_CODE_WIDTH;
        result.original_size = data.size();

        if (data.empty()) {
            result.codes = {};
            result.original_size = 0;
            return result;
        }

        // Инициализация словаря
        std::unordered_map<std::vector<uint8_t>, int> dictionary;
        for (int i = 0; i < 256; i++) {
            dictionary[{static_cast<uint8_t>(i)}] = i;
        }

        int next_code = 256;
        std::vector<uint8_t> w = {data[0]};
        std::vector<int> codes;

        for (size_t i = 1; i < data.size(); i++) {
            uint8_t byte = data[i];
            std::vector<uint8_t> k = {byte};
            
            // wk = w + k
            std::vector<uint8_t> wk = w;
            wk.insert(wk.end(), k.begin(), k.end());
            
            if (dictionary.find(wk) != dictionary.end()) {
                w = wk;
            } else {
                codes.push_back(dictionary[w]);
                if (next_code <= max_code) {
                    dictionary[wk] = next_code;
                    next_code++;
                }
                w = k;
            }
        }

        codes.push_back(dictionary[w]);
        result.codes = codes;
        return result;
    }

    static std::vector<uint8_t> decode(const LZWContainer& container, int max_code = DEFAULT_MAX_CODE) {
        const std::vector<int>& codes = container.codes;
        
        if (codes.empty()) {
            return {};
        }

        // Инициализация словаря
        std::unordered_map<int, std::vector<uint8_t>> dictionary;
        for (int i = 0; i < 256; i++) {
            dictionary[i] = {static_cast<uint8_t>(i)};
        }

        int next_code = 256;
        std::vector<uint8_t> w = dictionary[codes[0]];
        std::vector<uint8_t> out;
        out.insert(out.end(), w.begin(), w.end());

        for (size_t i = 1; i < codes.size(); i++) {
            int code = codes[i];
            std::vector<uint8_t> entry;
            
            if (dictionary.find(code) != dictionary.end()) {
                entry = dictionary[code];
            } else if (code == next_code) {
                // Особый случай LZW: код указывает на строку, которая создаётся прямо сейчас
                entry = w;
                entry.insert(entry.end(), w[0]);
            } else {
                throw std::runtime_error("Некорректный LZW-код: " + std::to_string(code));
            }

            out.insert(out.end(), entry.begin(), entry.end());
            
            if (next_code <= max_code) {
                std::vector<uint8_t> new_entry = w;
                new_entry.insert(new_entry.end(), entry[0]);
                dictionary[next_code] = new_entry;
                next_code++;
            }
            
            w = entry;
        }

        return out;
    }

    static int compressed_size_bits(const LZWContainer& container) {
        return container.codes.size() * container.code_width;
    }
};

// Вспомогательная функция для работы с сырыми байтами
LZWContainer encode_bytes(const uint8_t* data, size_t size, int max_code = 4095) {
    std::vector<uint8_t> vec(data, data + size);
    return LZW::encode(vec, max_code);
}

std::vector<uint8_t> decode_container(const LZWContainer& container, int max_code = 4095) {
    return LZW::decode(container, max_code);
}
