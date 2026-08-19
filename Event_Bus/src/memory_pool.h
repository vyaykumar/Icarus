#ifndef ICARUS_MEMORY_POOL_H
#define ICARUS_MEMORY_POOL_H
#include <array>
#include <cstdint>

template <typename T, uint32_t Capacity>
struct MemoryPool {
    struct Handle {
        uint32_t index;
        uint32_t generation;
    };

    std::array<T,Capacity> storage;
    std::array<uint32_t,Capacity> generations;
    std::array<bool, Capacity> available;
};

#endif //ICARUS_MEMORY_POOL_H
