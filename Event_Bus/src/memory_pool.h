#ifndef ICARUS_MEMORY_POOL_H
#define ICARUS_MEMORY_POOL_H
#include <array>
#include <cstdint>
#include <ranges>

struct Handle {
    uint32_t index;
    uint32_t generation;
};

template <typename T, uint32_t Capacity>
struct MemoryPool {

    std::array<T,Capacity> storage;
    std::array<uint32_t,Capacity> generations;
    std::array<bool, Capacity> available;

    // Can be replaced with a circular index marker variable.
    Handle allocate() {
        for (auto [idx, free] : std::views::enumerate(available)) {
            auto index = static_cast<uint32_t>(idx);
            if (free) {
                available[index] = false;
                ++generations[index];
                return {index, generations[index]};
            }
        }
        return {.index = UINT32_MAX, .generation = 0};
    }

    void deallocate(Handle handle) {
        if (handle.index < Capacity && generations[handle.index] == handle.generation) {
            available[handle.index] = true;
        }
    }

    T* get (Handle handle) {
        if (handle.index < Capacity && generations[handle.index] == handle.generation) {
            return &storage[handle.index];
        }
        return nullptr;
    }
};

#endif //ICARUS_MEMORY_POOL_H
