#pragma once

#include <string>

#include <cstdint>

struct File{
    char* contents;
    uint64_t size;
};

void OpenFile(const char* name, std::ifstream& file);
uint64_t GetFilesize(const char* name);
void ReadFile(std::ifstream file, char* buf, uint64_t size);