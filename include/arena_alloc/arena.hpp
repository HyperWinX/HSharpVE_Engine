#pragma once

#include <iostream>

namespace HSharpParser {
    class ArenaAllocator {
    private:
        size_t allocated_size;
        void* current_offset;
        void* mem_block_start;
    public:
        inline explicit ArenaAllocator(ArenaAllocator& alloc) = delete;
        inline ArenaAllocator operator=(ArenaAllocator& alloc) = delete;
        inline explicit ArenaAllocator(size_t bytes) : allocated_size(bytes) {
            mem_block_start = current_offset = malloc(bytes);
            if (!mem_block_start) {
                std::cerr << "ArenaAllocator failed to allocate memory: malloc() returned null" << std::endl;
                exit(1);
            }
        }
        inline ~ArenaAllocator() {
            free(mem_block_start);
            mem_block_start = current_offset = nullptr;
            allocated_size = 0;
        }

        template<typename T>
        inline T* alloc() {
            T* ptr = new(current_offset)T();
            current_offset += sizeof(T);
            return ptr;
        }

    };
}