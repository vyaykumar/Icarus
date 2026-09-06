#include <gtest/gtest.h>
#include <chrono>
#include <vector>
#include <algorithm>
#include <iostream>

#include "ingress/mock_ingress.h"
#include "ingress/event_director.h"
#include "transport/event_bus.h"
#include "publisher/publisher.h"

TEST(IngestionBenchmark, EventDirector_Latency_And_Throughput) {
    // TODO: Create MockIngressSource

    // TODO: Create EventBus<8192, 8192>

    // TODO: Create Publisher

    // TODO: Create EventDirector

    // TODO: Record start time

    // TODO: Start EventDirector

    // TODO: Wait for all 1000 events to be processed

    // TODO: Record end time

    // TODO: Calculate total time and throughput (events/sec)

    // TODO: Pop events from ring buffer and collect latency values

    // TODO: Sort latencies and calculate P50, P95, P99

    // TODO: Print results

    // TODO: Assert latency < 1µs and throughput > 1M/sec
}