#include "guarded_memory_pool.h"

template <typename T, uint32_t Capacity>
GuardedMemoryPool<T, Capacity>::GuardedMemoryPool() {
    const long page_size = sysconf(_SC_PAGESIZE);
    const size_t total_size = (Capacity * sizeof(T)) + page_size;
    
    storage_ = static_cast<T *>(mmap(nullptr, total_size,
                                     PROT_READ | PROT_WRITE,
                                     MAP_PRIVATE | MAP_ANONYMOUS,
                                     -1, 0));
    if (storage_ == MAP_FAILED) {
        throw std::runtime_error("mmap failed");
    }
    
    guard_page_ = static_cast<uint8_t *>(storage_) + (Capacity * sizeof(T));
    
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