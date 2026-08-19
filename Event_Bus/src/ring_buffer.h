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
        buffer[write_index++] = std::move(event);
    }

    Event pop () {
        auto temp = buffer[read_index];
        buffer[read_index++] = {};
        return std::move(temp);
    }

};



#endif //ICARUS_RING_BUFFER_H
