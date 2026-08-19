#ifndef ICARUS_RING_BUFFER_H
#define ICARUS_RING_BUFFER_H
#include <atomic>

#include "event.h"

template <uint32_t Capacity>
struct RingBuffer {
    Event buffer [Capacity];
    std::atomic<uint32_t> write_index {0};
    std::atomic<uint32_t> read_index {0};

    void push (const Event& event) {
        uint32_t idx = write_index.load(std::memory_order_relaxed);
        buffer[idx] = event;
        write_index.store((idx+1) % Capacity, std::memory_order_release);
    }

    Event pop () {
        uint32_t idx = read_index.load(std::memory_order_acquire);
        const Event temp = buffer[idx];
        read_index.store((idx+1) % Capacity, std::memory_order_release);
        return temp;
    }

};



#endif //ICARUS_RING_BUFFER_H
