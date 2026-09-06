#include <gtest/gtest.h>
#include <chrono>
#include <vector>
#include <algorithm>
#include <iostream>

#include "latency_utils.h"
#include "ingress/mock_ingress.h"
#include "ingress/event_director.h"
#include "transport/event_bus.h"
#include "publisher/publisher.h"

TEST(IngestionBenchmark, EventDirector_Latency_And_Throughput) {
    MockIngressSource ingress;
    EventBus<8192, 8192> event_bus;
    Publisher publisher (event_bus);
    EventDirector director (ingress, publisher, event_bus);

    const auto start = std::chrono::high_resolution_clock::now().time_since_epoch().count();

    director.start();
    while (director.sequence_count() < 1000)
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    director.wait();

    const auto end = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    const auto elapsed = end - start;
    // const auto latency {elapsed/1000};
    double throughput = (1000.0 / elapsed) * 1e9;

    std::vector<uint64_t> latencies;
    Event event;
    uint64_t first_timestamp = 0;

    while (event_bus.pop(event)) {
        if (first_timestamp == 0)
            first_timestamp = event.nano_stamp;

        uint64_t event_latency = event.nano_stamp - first_timestamp;
        latencies.push_back(event_latency);
    }

    uint64_t p50 = calculate_percentile_linear(latencies, 50.0);
    uint64_t p95 = calculate_percentile_linear(latencies, 95.0);
    uint64_t p99 = calculate_percentile_linear(latencies, 99.0);

    std::cout << "\n=== Ingestion Benchmark Results ===\n";
    std::cout << "Events processed: " << director.sequence_count() << "\n";
    std::cout << "Total time: " << elapsed << " ns\n";
    std::cout << "Throughput: " << throughput << " events/sec\n";
    std::cout << "P50 latency: " << p50 << " ns\n";
    std::cout << "P95 latency: " << p95 << " ns\n";
    std::cout << "P99 latency: " << p99 << " ns\n\n";

    EXPECT_LT(p50, 100000);   // 100µs (WSL overhead)
    EXPECT_GT(throughput, 1000);  // 1K events/sec (realistic for WSL)
}