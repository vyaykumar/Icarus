#include <gtest/gtest.h>

#include <chrono>
#include <thread>

#include "../src/ring_buffer.h"
#include "../src/memory_pool.h"
#include "../src/event_bus.h"

TEST(PerformanceBenchmark, LatencyAndThroughput) {
    constexpr uint32_t buffer_size = 1024*1;
    RingBuffer<buffer_size> buffer;

    const auto start = std::chrono::high_resolution_clock::now();

    auto producer = std::jthread([&buffer] {
        for (uint32_t idx {}; idx < buffer_size; ++idx) {
            Event event {.nano_stamp = idx};
            while(!buffer.push(event));
        }
    });

    auto consumer = std::jthread([&buffer] {
        for (uint32_t idx {}; idx < buffer_size; ++idx) {
            Event event {};
            while(!buffer.pop(event));
        }
    });

    const auto end = std::chrono::high_resolution_clock::now();
    const auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end-start);

    const auto latency_per_event = elapsed/buffer_size;

    double throughput_eps = (buffer_size * 1e9) / elapsed.count();

    std::cout << "Latency per event: " << latency_per_event.count() << " ns\n";
    std::cout << "Throughput: " << throughput_eps << " events/sec\n";

    ASSERT_TRUE(latency_per_event <= std::chrono::nanoseconds(500));
    ASSERT_TRUE(throughput_eps >= 1e6);
}