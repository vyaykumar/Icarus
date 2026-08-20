#include <gtest/gtest.h>

#include <chrono>
#include <thread>

#include "../src/ring_buffer.h"
#include "../src/memory_pool.h"
#include "../src/event_bus.h"

TEST(PerformanceBenchmark, LatencyAndThroughput) {
    constexpr uint32_t buffer_size = 1024*1;
    RingBuffer<buffer_size> buffer;

    std::atomic<bool> producer_ready{false};
    std::atomic<bool> consumer_ready{false};

    double latency;
    double throughput;

    auto producer = std::jthread([&] {
        producer_ready.store(true);
        while (!consumer_ready.load()) {}

        const auto start = std::chrono::high_resolution_clock::now();

        for (uint32_t idx {}; idx < buffer_size; ++idx) {
            Event event {.nano_stamp = idx};
            while (!buffer.push(event)) {}
        }

        const auto end = std::chrono::high_resolution_clock::now();
        const auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);

        latency = static_cast<double>(elapsed.count()) / buffer_size;
        throughput = (buffer_size * 1e9) / elapsed.count();
    });

    auto consumer = std::jthread([&] {
        consumer_ready.store(true);
        while (!producer_ready.load()) {}

        for (uint32_t idx {}; idx < buffer_size; ++idx) {
            Event event {};
            while(!buffer.pop(event));
        }
    });

    std::cout << "Latency: " << latency << " ns\n";
    std::cout << "Throughput: " << throughput << " events/sec\n";

    ASSERT_TRUE(latency <= 500);
    ASSERT_TRUE(throughput >= 1e6);
}