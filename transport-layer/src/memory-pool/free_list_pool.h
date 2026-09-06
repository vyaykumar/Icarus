#ifndef ICARUS_FREE_LIST_POOL_H
#define ICARUS_FREE_LIST_POOL_H

#include <array>
#include <cstdint>
#include <ranges>
#include <stdexcept>
#include <vector>

template <typename T, uint32_t Capacity>
class FreeListPool {
private:
    std::array<T, Capacity> storage_;
    std::array<uint32_t, Capacity> generations_;
    std::array<uint32_t, Capacity> free_stack_;
    uint32_t free_count_;

public:
    FreeListPool();

    Handle allocate();
    void deallocate(Handle handle);
    T* get(Handle handle);
};

template <typename T, uint32_t Capacity>
FreeListPool<T, Capacity>::FreeListPool() {
    generations_.fill(0);

    auto iota_view = std::views::iota(0u, Capacity);
    std::copy(iota_view.begin(), iota_view.end(), free_stack_.begin());

    free_count_ = Capacity;
}

template <typename T, uint32_t Capacity>
Handle FreeListPool<T, Capacity>::allocate() {
    if (free_count_ <= 0) return {.index = UINT32_MAX, .generation = 0};

    uint32_t index = free_stack_[free_count_ - 1];
    --free_count_;
    ++generations_[index];

    return {.index = index, .generation = generations_[index]};
}

template <typename T, uint32_t Capacity>
void FreeListPool<T, Capacity>::deallocate(Handle handle) {
    if (handle.index >= Capacity) return;

    if (generations_[handle.index] != handle.generation) return;

    free_stack_[free_count_] = handle.index;
    ++free_count_;
}

template <typename T, uint32_t Capacity>
T* FreeListPool<T, Capacity>::get(Handle handle) {
    if (handle.index >= Capacity) return nullptr;
    if (generations_[handle.index] != handle.generation) return nullptr;

    return &storage_[handle.index];
}

#endif //ICARUS_FREE_LIST_POOL_H
