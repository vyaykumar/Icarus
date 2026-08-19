#ifndef ICARUS_EVENT_BUS_H
#define ICARUS_EVENT_BUS_H

#include <cstdint>
#include "ring_buffer.h"
#include "memory_pool.h"

template <uint32_t BufferCapacity, uint32_t PayloadCapacity>
struct EventBus {
    RingBuffer<BufferCapacity> command_buffer;
    RingBuffer<BufferCapacity> response_buffer;
    MemoryPool<uint8_t, PayloadCapacity> payload_pool;

    uint32_t latency_ns;
    uint32_t jitter_ns;
    float packet_loss_rate;
};

#endif //ICARUS_EVENT_BUS_H
