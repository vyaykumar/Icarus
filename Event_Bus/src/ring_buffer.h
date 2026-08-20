#ifndef ICARUS_RING_BUFFER_H
#define ICARUS_RING_BUFFER_H

#include <atomic>
#include <cmath>

#include "event.h"

template <uint32_t Capacity>
struct RingBuffer {
    static_assert((Capacity & Capacity-1) == 0, "Capacity must be a power of 2.");
    static constexpr uint32_t MASK = Capacity - 1;

    Event buffer [Capacity];
    std::atomic<uint32_t> write_index {0};
    std::atomic<uint32_t> read_index {0};

    bool push (const Event& event) {
        const uint32_t w_idx = write_index.load(std::memory_order_relaxed);
        const uint32_t r_idx = read_index.load(std::memory_order_acquire);
        const uint32_t next_w_idx = (w_idx+1) & MASK;

        if (next_w_idx == r_idx) return false;

        buffer[w_idx & MASK] = event;
        write_index.store(next_w_idx, std::memory_order_release);
        return true;
    }

    bool pop(Event& event) {
        const uint32_t w_idx = write_index.load(std::memory_order_relaxed);
        const uint32_t r_idx = read_index.load(std::memory_order_acquire);

        if (r_idx == w_idx) return false;

        event = buffer[r_idx & MASK];
        read_index.store((r_idx + 1) & MASK, std::memory_order_release);
        return true;
    }

    [[nodiscard]] bool is_empty() const {
        const uint32_t w_idx = write_index.load(std::memory_order_relaxed);
        const uint32_t r_idx = read_index.load(std::memory_order_acquire);

        return r_idx == w_idx;
    }

    [[nodiscard]] bool is_full () const {
        const uint32_t w_idx = write_index.load(std::memory_order_relaxed);
        const uint32_t r_idx = read_index.load(std::memory_order_acquire);

        const uint32_t next_w_idx = (w_idx+1) & MASK;

        return next_w_idx == r_idx;
    }

    [[nodiscard]] uint32_t size() const {
        const uint32_t w_idx = write_index.load(std::memory_order_relaxed);
        const uint32_t r_idx = read_index.load(std::memory_order_acquire);

        return (w_idx - r_idx + Capacity) & MASK;
    }
};



#endif //ICARUS_RING_BUFFER_H
