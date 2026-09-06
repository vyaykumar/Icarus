#ifndef ICARUS_GUARDED_MEMORY_POOL_H
#define ICARUS_GUARDED_MEMORY_POOL_H

#include <cstdint>
#include <sys/mman.h>
#include <stdexcept>
#include <unistd.h>

template <typename T, uint32_t Capacity>
class GuardedMemoryPool {
private:
    T* storage_;
    uint8_t* guard_page_;

public:
    GuardedMemoryPool();
    ~GuardedMemoryPool();
    T* get(uint32_t index);
};

template <typename T, uint32_t Capacity>
GuardedMemoryPool<T, Capacity>::GuardedMemoryPool() {
    const long page_size = sysconf(_SC_PAGESIZE);
    const size_t pool_size = Capacity * sizeof(T);
    const size_t total_size = pool_size + page_size;

    storage_ = static_cast<T *>(mmap(nullptr, total_size,
                                     PROT_READ | PROT_WRITE,
                                     MAP_PRIVATE | MAP_ANONYMOUS,
                                     -1, 0));
    if (storage_ == MAP_FAILED) {
        throw std::runtime_error("mmap failed");
    }

    // Align guard page to page boundary
    uintptr_t pool_end = reinterpret_cast<uintptr_t>(storage_) + pool_size;
    uintptr_t aligned_guard = (pool_end + page_size - 1) & ~(page_size - 1);
    guard_page_ = reinterpret_cast<uint8_t *>(aligned_guard);

    if (mprotect(guard_page_, page_size, PROT_NONE) != 0) {
        munmap(storage_, total_size);
        throw std::runtime_error("mprotect failed");
    }
}

template <typename T, uint32_t Capacity>
GuardedMemoryPool<T, Capacity>::~GuardedMemoryPool() {
    const long page_size = sysconf(_SC_PAGESIZE);
    const size_t total_size = (Capacity * sizeof(T)) + page_size;
    munmap(storage_, total_size);
}

template <typename T, uint32_t Capacity>
T* GuardedMemoryPool<T, Capacity>::get(uint32_t index) {
    if (index >= Capacity) {
        return static_cast<T *>(guard_page_);
    }
    return storage_ + index;
}

#endif //ICARUS_GUARDED_MEMORY_POOL_H
