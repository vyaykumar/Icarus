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
    double throughput = (1000.0 / elapsed) * 1e9;

    auto latencies = director.get_latencies();

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

    EXPECT_LT(p50, 1000);      // 1µs (sub-microsecond)
    EXPECT_GT(throughput, 10000);  // 10K events/sec (WSL constraint)

    // EXPECT_LT(p50, 100000);   // 100µs (WSL overhead)
    // EXPECT_GT(throughput, 1000);  // 1K events/sec (realistic for WSL)
}
/// With WSL overhead
/// Subscriber busy-waiting
// === Ingestion Benchmark Results ===
// EventBus size = 8192
// Events processed: 1000
// Total time: 101360499 ns
// Throughput: 9865.78 events/sec
// P50 latency: 110 ns
// P95 latency: 272 ns
// P99 latency: 608 ns

/// With WSL overhead
/// Subscriber yielding
// === Ingestion Benchmark Results === (With WSL overhead, but )
// Events processed: 1000
// Total time: 91369787 ns
// Throughput: 10944.5 events/sec
// P50 latency: 61 ns
// P95 latency: 175 ns
// P99 latency: 336 ns

/// With WSL overhead
/// Subscriber yielding
/// 16384 EventBus, previously 8192
/// 10ns Director sleep, previously 100ns
// === Ingestion Benchmark Results ===
// Events processed: 1000
// Total time: 91742703 ns
// Throughput: 10900 events/sec
// P50 latency: 56 ns
// P95 latency: 248 ns
// P99 latency: 531 ns

/// With WSL overhead
/// FreeListMemory instead of O(N) linear search.
/// Subscriber yielding
/// 8192 EventBus
/// Director 10ns sleep
// === Ingestion Benchmark Results ===
// Events processed: 1000
// Total time: 81563305 ns
// Throughput: 12260.4 events/sec   (12% faster)
// P50 latency: 33 ns               (41% faster)
// P95 latency: 173 ns              (30% faster)
// P99 latency: 336 ns              (37% faster)

/// With WSL overhead
/// FreeListMemory
/// Subscriber yielding
/// 8192 EventBus
/// Director sleep, refactored with _mm_pause();
// === Ingestion Benchmark Results ===
// Events processed: 1000
// Total time: 10579635 ns
// Throughput: 94521.2 events/sec   (671% faster)
// P50 latency: 24 ns               (27% faster)
// P95 latency: 25 ns               (85% faster)
// P99 latency: 35 ns               (90% faster)