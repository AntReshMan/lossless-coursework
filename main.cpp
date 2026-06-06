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
    std::cout << "Examples:" << std::endl;
    std::cout << "  " << program_name << " huffman input.txt" << std::endl;
    std::cout << "  " << program_name << " lzw input.txt" << std::endl;
}

void run(const std::string& algorithm, const fs::path& path) {
    // Проверяем существование файла
    if (!fs::exists(path)) {
        throw std::runtime_error("File not found: " + path.string());
    }
    
    // Читаем файл
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Can't open file: " + path.string());
    }
    
    std::vector<uint8_t> data((std::istreambuf_iterator<char>(file)), 
                               std::istreambuf_iterator<char>());
    file.close();
    
    std::cout << "Original size: " << data.size() << "bytes" << std::endl;
    
    // Выбираем алгоритм
    if (algorithm == "huffman") {
        std::cout << "Algorithm: Huffman" << std::endl;
        
        auto packed = Huffman::encode(data);
        auto restored = Huffman::decode(packed);
        
        if (restored != data) {
            throw std::runtime_error("Decode error");
        }
        
        size_t original_bits = data.size() * 8;
        int compressed_bits = Huffman::compressed_size_bits(packed);
        double ratio = (original_bits > 0) ? 
                       static_cast<double>(compressed_bits) / original_bits : 0;
        double saved = (original_bits > 0) ? 100 * (1 - ratio) : 0;
        
        std::cout << "Compressed size: " << compressed_bits << "bits" << std::endl;
        std::cout << "Coefficient compressed/original: " << std::fixed 
                  << std::setprecision(3) << ratio << std::endl;
        std::cout << "Economy: " << std::fixed << std::setprecision(1) 
                  << saved << "%" << std::endl;
                  
    } else if (algorithm == "lzw") {
        std::cout << "Algorithm: LZW" << std::endl;
        
        auto packed = LZW::encode(data);
        auto restored = LZW::decode(packed);
        
        if (restored != data) {
            throw std::runtime_error("Decode error");
        }
        
        size_t original_bits = data.size() * 8;
        int compressed_bits = LZW::compressed_size_bits(packed);
        double ratio = (original_bits > 0) ? 
                       static_cast<double>(compressed_bits) / original_bits : 0;
        double saved = (original_bits > 0) ? 100 * (1 - ratio) : 0;
        
        std::cout << "Evaluated size: " << compressed_bits << "bits" << std::endl;
        std::cout << "Coefficient compressed/original: " << std::fixed 
                  << std::setprecision(3) << ratio << std::endl;
        std::cout << "Economy: " << std::fixed << std::setprecision(1) 
                  << saved << "%" << std::endl;
    } else {
        throw std::runtime_error("Unknown algorithm: " + algorithm);
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
            std::cerr << "ОErrot: algorithm must be Huffman or LZW" << std::endl;
            print_usage(argv[0]);
            return 1;
        }
        
        run(algorithm, input_path);
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
