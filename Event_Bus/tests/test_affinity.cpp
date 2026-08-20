#include <gtest/gtest.h>
#include <thread>
#include "../src/core/affinity.h"

using namespace icarus::core;

TEST(AffinityTest, BindThreadToCore) {
    size_t counter {0};
    size_t limit {1000};

    auto worker = std::thread([&]{ while (limit--) counter++; });

    ASSERT_NO_THROW(bind_thread_to_core(worker, 1););
    worker.join();
    ASSERT_EQ(counter, 1000);
}

TEST(AffinityTest, BindThreadInvalidCore) {
    auto worker = std::thread([] { /*nothing in here*/ });
    ASSERT_THROW(bind_thread_to_core(worker, -1), std::runtime_error);
    worker.join();
}