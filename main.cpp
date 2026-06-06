#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
#include <filesystem>
#include <cassert>
#include <stdexcept>

#include "huffman.h"
#include "lzw.h"

namespace fs = std::filesystem;

void print_usage(const char* program_name) {
    std::cout << "Примеры использования:" << std::endl;
    std::cout << "  " << program_name << " huffman input.txt" << std::endl;
    std::cout << "  " << program_name << " lzw input.txt" << std::endl;
}

void run(const std::string& algorithm, const fs::path& path) {
    // Проверяем существование файла
    if (!fs::exists(path)) {
        throw std::runtime_error("Файл не найден: " + path.string());
    }
    
    // Читаем файл
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Не удалось открыть файл: " + path.string());
    }
    
    std::vector<uint8_t> data((std::istreambuf_iterator<char>(file)), 
                               std::istreambuf_iterator<char>());
    file.close();
    
    std::cout << "Исходный размер: " << data.size() << " байт" << std::endl;
    
    // Выбираем алгоритм
    if (algorithm == "huffman") {
        std::cout << "Алгоритм: Huffman" << std::endl;
        
        auto packed = Huffman::encode(data);
        auto restored = Huffman::decode(packed);
        
        if (restored != data) {
            throw std::runtime_error("Декодирование не совпало с исходным файлом");
        }
        
        size_t original_bits = data.size() * 8;
        int compressed_bits = Huffman::compressed_size_bits(packed);
        double ratio = (original_bits > 0) ? 
                       static_cast<double>(compressed_bits) / original_bits : 0;
        double saved = (original_bits > 0) ? 100 * (1 - ratio) : 0;
        
        std::cout << "Оценка сжатого размера: " << compressed_bits << " бит" << std::endl;
        std::cout << "Коэффициент compressed/original: " << std::fixed 
                  << std::setprecision(3) << ratio << std::endl;
        std::cout << "Экономия: " << std::fixed << std::setprecision(1) 
                  << saved << "%" << std::endl;
                  
    } else if (algorithm == "lzw") {
        std::cout << "Алгоритм: LZW" << std::endl;
        
        auto packed = LZW::encode(data);
        auto restored = LZW::decode(packed);
        
        if (restored != data) {
            throw std::runtime_error("Декодирование не совпало с исходным файлом");
        }
        
        size_t original_bits = data.size() * 8;
        int compressed_bits = LZW::compressed_size_bits(packed);
        double ratio = (original_bits > 0) ? 
                       static_cast<double>(compressed_bits) / original_bits : 0;
        double saved = (original_bits > 0) ? 100 * (1 - ratio) : 0;
        
        std::cout << "Оценка сжатого размера: " << compressed_bits << " бит" << std::endl;
        std::cout << "Коэффициент compressed/original: " << std::fixed 
                  << std::setprecision(3) << ratio << std::endl;
        std::cout << "Экономия: " << std::fixed << std::setprecision(1) 
                  << saved << "%" << std::endl;
    } else {
        throw std::runtime_error("Неизвестный алгоритм: " + algorithm);
    }
}

int main(int argc, char* argv[]) {
    try {
        if (argc != 3) {
            print_usage(argv[0]);
            return 1;
        }
        
        std::string algorithm = argv[1];
        fs::path input_path = argv[2];
        
        if (algorithm != "huffman" && algorithm != "lzw") {
            std::cerr << "Ошибка: алгоритм должен быть 'huffman' или 'lzw'" << std::endl;
            print_usage(argv[0]);
            return 1;
        }
        
        run(algorithm, input_path);
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
        return 1;
    }
}
