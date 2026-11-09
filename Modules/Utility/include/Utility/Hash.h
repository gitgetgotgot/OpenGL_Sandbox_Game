#pragma once
#include <cstdint>
#include <cstddef>
#include <vector>
#include <string>

namespace Engine::Util {
    class Hash {
    public:
        inline static uint64_t FNV_1a_64(const uint8_t* data, size_t size) {
            // 64-bit const FNV-1a
            constexpr uint64_t FNV_OFFSET_BASIS = 14695981039346656037ULL;
            constexpr uint64_t FNV_PRIME = 1099511628211ULL;

            uint64_t hash = FNV_OFFSET_BASIS;
            for (size_t i = 0; i < size; ++i) {
                hash ^= data[i];
                hash *= FNV_PRIME;
            }
            return hash;
        }
        inline static uint64_t FNV_1a_64(const std::vector<uint8_t>& buffer) {
            return FNV_1a_64(buffer.data(), buffer.size());
        }
    };
}