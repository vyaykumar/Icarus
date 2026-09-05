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

#endif //ICARUS_GUARDED_MEMORY_POOL_H
