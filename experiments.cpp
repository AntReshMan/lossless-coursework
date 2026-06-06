#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <random>
#include <iomanip>
#include <filesystem>
#include <cassert>

#include "huffman.h"
#include "lzw.h"

namespace fs = std::filesystem;

// Структура для хранения набора данных
struct Dataset {
    std::string name;
    std::vector<uint8_t> data;
};

std::vector<uint8_t> generate_repeating_text() {
    std::string text = "ABRACADABRA ";
    std::string result;
    for (int i = 0; i < 600; i++) {
        result += text;
    }
    return std::vector<uint8_t>(result.begin(), result.end());
}

std::vector<uint8_t> generate_russian_fragment() {
    std::string text = "сжатие без потерь алгоритм хаффмана lzw ";
    std::string result;
    for (int i = 0; i < 350; i++) {
        result += text;
    }
    return std::vector<uint8_t>(result.begin(), result.end());
}

std::vector<uint8_t> generate_random_bytes() {
    std::mt19937 rng(42);  // Фиксированное seed для воспроизводимости
    std::uniform_int_distribution<uint8_t> dist(0, 255);
    
    std::vector<uint8_t> result;
    result.reserve(6000);
    for (int i = 0; i < 6000; i++) {
        result.push_back(dist(rng));
    }
    return result;
}

class Experiments {
private:
    std::vector<Dataset> datasets;

public:
    Experiments() {
        datasets = {
            {"Повторяющийся текст", generate_repeating_text()},
            {"Русский фрагмент", generate_russian_fragment()},
            {"Псевдослучайные байты", generate_random_bytes()}
        };
    }

    void run() {
        // Создаем директорию для результатов
        fs::create_directories("results");
        std::string out_path = "results/experiment_results.csv";
        
        std::ofstream file(out_path);
        if (!file.is_open()) {
            throw std::runtime_error("Не удалось создать файл: " + out_path);
        }
        
        // Записываем заголовок CSV
        file << "dataset,algorithm,original_bits,compressed_bits,ratio,saved_percent\n";
        
        // Для каждого набора данных
        for (const auto& dataset : datasets) {
            std::cout << "Обработка: " << dataset.name << std::endl;
            
            // Тестируем Huffman
            try {
                auto huffman_container = Huffman::encode(dataset.data);
                auto huffman_decoded = Huffman::decode(huffman_container);
                assert(dataset.data == huffman_decoded);
                
                size_t original_bits = dataset.data.size() * 8;
                int compressed_bits = Huffman::compressed_size_bits(huffman_container);
                double ratio = static_cast<double>(compressed_bits) / original_bits;
                double saved_percent = 100 * (1 - ratio);
                
                file << "\"" << dataset.name << "\",Huffman,"
                     << original_bits << ","
                     << compressed_bits << ","
                     << std::fixed << std::setprecision(4) << ratio << ","
                     << std::fixed << std::setprecision(2) << saved_percent << "\n";
                
                std::cout << "  Huffman: " << original_bits << " -> " 
                          << compressed_bits << " бит (экономия: " 
                          << saved_percent << "%)" << std::endl;
                          
            } catch (const std::exception& e) {
                std::cerr << "  Ошибка Huffman: " << e.what() << std::endl;
            }
            
            // Тестируем LZW
            try {
                auto lzw_container = LZW::encode(dataset.data);
                auto lzw_decoded = LZW::decode(lzw_container);
                assert(dataset.data == lzw_decoded);
                
                size_t original_bits = dataset.data.size() * 8;
                int compressed_bits = LZW::compressed_size_bits(lzw_container);
                double ratio = static_cast<double>(compressed_bits) / original_bits;
                double saved_percent = 100 * (1 - ratio);
                
                file << "\"" << dataset.name << "\",LZW,"
                     << original_bits << ","
                     << compressed_bits << ","
                     << std::fixed << std::setprecision(4) << ratio << ","
                     << std::fixed << std::setprecision(2) << saved_percent << "\n";
                
                std::cout << "  LZW: " << original_bits << " -> " 
                          << compressed_bits << " бит (экономия: " 
                          << saved_percent << "%)" << std::endl;
                          
            } catch (const std::exception& e) {
                std::cerr << "  Ошибка LZW: " << e.what() << std::endl;
            }
            
            std::cout << std::endl;
        }
        
        file.close();
        std::cout << "Результаты записаны в " << out_path << std::endl;
    }
};

int main() {
    try {
        Experiments exp;
        exp.run();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
        return 1;
    }
}
