#ifndef ICARUS_RING_BUFFER_H
#define ICARUS_RING_BUFFER_H

#include <atomic>
#include <cmath>

#include "event.h"

template <uint32_t Capacity>
struct RingBuffer {
    Event buffer [Capacity];
    std::atomic<uint32_t> write_index {0};
    std::atomic<uint32_t> read_index {0};

    bool push (const Event& event) {
        const uint32_t w_idx = write_index.load(std::memory_order_relaxed);
        const uint32_t r_idx = read_index.load(std::memory_order_acquire);
        const uint32_t next_w_idx = (w_idx+1) % Capacity;

        if (next_w_idx == r_idx) return false;

        buffer[w_idx] = event;
        write_index.store((w_idx+1) % Capacity, std::memory_order_release);
        return true;
    }

    bool pop(Event& event) {
        const uint32_t w_idx = write_index.load(std::memory_order_relaxed);
        const uint32_t r_idx = read_index.load(std::memory_order_acquire);

        if (r_idx == w_idx) return false;

        event = buffer[r_idx];
        read_index.store((r_idx + 1) % Capacity, std::memory_order_release);
        return true;
    }

    bool is_empty() const {
        const uint32_t w_idx = write_index.load(std::memory_order_relaxed);
        const uint32_t r_idx = read_index.load(std::memory_order_acquire);

        return r_idx == w_idx;
    }

    bool is_full () const {
        const uint32_t w_idx = write_index.load(std::memory_order_relaxed);
        const uint32_t r_idx = read_index.load(std::memory_order_acquire);

        const uint32_t next_w_idx = (w_idx+1) % Capacity;

        return next_w_idx == r_idx;
    }

    uint32_t size() const {
        const uint32_t w_idx = write_index.load(std::memory_order_relaxed);
        const uint32_t r_idx = read_index.load(std::memory_order_acquire);

        return (w_idx - r_idx + Capacity) % Capacity;
    }
};



#endif //ICARUS_RING_BUFFER_H
