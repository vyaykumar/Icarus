#ifndef ICARUS_EVENT_BUS_H
#define ICARUS_EVENT_BUS_H

#include <cstdint>
#include <random>
#include <bits/this_thread_sleep.h>

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

    void send(const Event& event) {
        static std::mt19937 gen( std::random_device{}() );
        std::uniform_real_distribution loss_dist(0.0f, 1.0f);

        // Packet loss.
        if (loss_dist(gen) < packet_loss_rate)
            return;

        // Delay lama.
        std::uniform_int_distribution<uint32_t> jitter_dist (0, jitter_ns);
        const uint32_t total_delay = latency_ns + jitter_dist(gen);

        std::this_thread::sleep_for(std::chrono::nanoseconds(total_delay));

        command_buffer.push(event);
    }

    bool receive (Event& event) {
        static std::mt19937 gen( std::random_device{}() );
        std::uniform_real_distribution loss_dist(0.0f, 1.0f);

        // Packet loss.
        if (loss_dist(gen) < packet_loss_rate)
            return false;

        // Delay.
        std::uniform_int_distribution<uint32_t> jitter_dist (0, jitter_ns);
        const uint32_t total_delay = latency_ns + jitter_dist(gen);

        std::this_thread::sleep_for(std::chrono::nanoseconds(total_delay));

        return response_buffer.pop(event);
    }

    Handle allocate_payload() {
        return payload_pool.allocate();
    }

    void deallocate_payload (Handle handle) {
        payload_pool.deallocate(handle);
    }

    void push (const Event& event) {
        command_buffer.push(event);
    }

    bool pop (Event& event) {
        return command_buffer.pop(event);
    }

    uint8_t* get_payload(Handle handle) {
        return payload_pool.get(handle);
    }
};

#endif //ICARUS_EVENT_BUS_H
