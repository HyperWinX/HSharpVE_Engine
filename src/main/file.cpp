#include <fstream>
#include <iostream>
#include <filesystem>

#include <cstdint>

void OpenFile(const char* name, std::ifstream& file){
    file = std::ifstream(name, std::ios::in);
    if (!file.is_open()){
        std::cerr << "Invalid filename, cannot be opened." << std::endl;
        exit(EXIT_FAILURE);
    }
}

uint64_t GetFilesize(const char* name){
    return std::filesystem::file_size(name);
}

void ReadFile(std::ifstream file, char* buf, uint64_t size){
    file.read(buf, size);
}