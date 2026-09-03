#include <gtest/gtest.h>

#include <chrono>
#include <thread>

#include "../src/ring_buffer.h"
#include "../src/transport/memory_pool.h"
#include "../src/transport/event_bus.h"
#include "latency_utils.h"

TEST(PerformanceBenchmark, EndToEndLatencyAndThroughput) {
    constexpr uint32_t NUM_EVENTS = 1'000'000;
    constexpr uint32_t BUFFER_SIZE = 1024 * 1;

    RingBuffer<BUFFER_SIZE> buffer;

    std::vector<uint64_t> enqueue_times(NUM_EVENTS);
    std::vector<uint64_t> dequeue_times(NUM_EVENTS);
    std::vector<uint64_t> latencies(NUM_EVENTS);
    std::atomic<uint32_t> consumed{0};

    const auto start_total = std::chrono::steady_clock::now();

    auto producer = std::jthread([&]() {
        for (uint32_t i = 0; i < NUM_EVENTS; ++i) {
            enqueue_times[i] = std::chrono::steady_clock::now()
                              .time_since_epoch().count();

            Event event{.nano_stamp = enqueue_times[i]};

            while (!buffer.push(event))
                std::this_thread::yield();
        }
    });

    auto consumer = std::jthread([&]() {
        uint32_t count = 0;
        while (count < NUM_EVENTS) {
            Event event{};
            if (buffer.pop(event)) {
                dequeue_times[count] = std::chrono::steady_clock::now()
                                      .time_since_epoch().count();
                latencies[count] = dequeue_times[count] - enqueue_times[count];
                count++;
                consumed.store(count, std::memory_order_release);
            }
            else
                std::this_thread::yield();
        }
    });

    while (consumed.load(std::memory_order_acquire) < NUM_EVENTS)
        std::this_thread::yield();

    const auto end_total = std::chrono::steady_clock::now();

    // Calculate statistics
    uint64_t total_latency = 0;
    uint64_t min_latency = UINT64_MAX;
    uint64_t max_latency = 0;

    for (uint32_t i = 0; i < NUM_EVENTS; ++i) {
        uint64_t lat = latencies[i];
        total_latency += lat;
        min_latency = std::min(min_latency, lat);
        max_latency = std::max(max_latency, lat);
    }

    double avg_latency = static_cast<double>(total_latency) / NUM_EVENTS;
    double throughput = (NUM_EVENTS * 1e9) /
        std::chrono::duration_cast<std::chrono::nanoseconds>(end_total - start_total).count();

    // Calculate percentiles
    const uint64_t p50 = calculate_percentile(latencies, 50);
    const uint64_t p90 = calculate_percentile(latencies, 90);
    const uint64_t p95 = calculate_percentile(latencies, 95);
    const uint64_t p99 = calculate_percentile(latencies, 99);
    const uint64_t p999 = calculate_percentile(latencies, 99.9);

    // Output results
    std::cout << "\n";
    std::cout << "Number of Events: " << NUM_EVENTS << ".\n";
    std::cout << "Buffer Size: "<< BUFFER_SIZE*64 << " bytes.\n\n";
    std::cout << "Average latency:\t" << avg_latency << " ns\n";
    std::cout << "Min latency:\t\t" << min_latency << " ns\n";
    std::cout << "Max latency:\t\t" << max_latency << " ns\n\n";
    std::cout << "P50 (median):\t\t" << p50 << " ns\n";
    std::cout << "P90:\t\t\t\t" << p90 << " ns\n";
    std::cout << "P95:\t\t\t\t" << p95 << " ns\n";
    std::cout << "P99:\t\t\t\t" << p99 << " ns\n";
    std::cout << "P99.9:\t\t\t\t" << p999 << " ns\n\n";
    std::cout << "Throughput:\t\t\t" << throughput << " events/sec\n";
    std::cout << "Total time:\t\t\t" << (end_total - start_total).count() / 1e9 << " seconds\n";

    // Compute percentiles manually for verification
    std::cout << "\n=== LATENCY DISTRIBUTION ===\n";
    auto sorted = latencies;

    std::ranges::sort(sorted);

    std::cout << "Events < 1µs: "
              << std::count_if(sorted.begin(), sorted.end(), [](const uint64_t x) { return x < 1000; })
              << " (" << (100.0 * std::count_if(sorted.begin(), sorted.end(),
                  [](uint64_t x) { return x < 1000; }) / NUM_EVENTS) << "%)\n";
    std::cout << "Events < 10µs: "
              << std::count_if(sorted.begin(), sorted.end(), [](const uint64_t x) { return x < 10000; })
              << " (" << (100.0 * std::count_if(sorted.begin(), sorted.end(),
                  [](uint64_t x) { return x < 10000; }) / NUM_EVENTS) << "%)\n";
    std::cout << "Events < 100µs: "
              << std::count_if(sorted.begin(), sorted.end(), [](const uint64_t x) { return x < 100000; })
              << " (" << (100.0 * std::count_if(sorted.begin(), sorted.end(),
                  [](uint64_t x) { return x < 100000; }) / NUM_EVENTS) << "%)\n";
    std::cout << "Events < 1ms: "
              << std::count_if(sorted.begin(), sorted.end(), [](const uint64_t x) { return x < 1000000; })
              << " (" << (100.0 * std::count_if(sorted.begin(), sorted.end(),
                  [](uint64_t x) { return x < 1000000; }) / NUM_EVENTS) << "%)\n\n";
}