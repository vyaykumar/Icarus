#include <gtest/gtest.h>
#include <thread>
#include "../src/core/affinity.h"

using namespace icarus::core;

TEST(AffinityTest, BindThreadToCore) {
    /// TODO: Create an int counter initialized to 0.
    size_t counter {0};
    size_t limit {1000};

    /// TODO: Create a std::thread with a lambda that increments the counter 1000 times.
    auto worker = std::thread([&]{ while (limit--) counter++; });

    /// TODO: Call bind_thread_to_core(thread, 1).
    bind_thread_to_core(worker, 1);

    /// TODO: Assert no exception is thrown.
    ASSERT_NO_THROW();

    /// TODO: Join the thread.
    worker.join();

    /// TODO: Assert counter == 1000.
    ASSERT_EQ(counter == 1000);
}

TEST(AffinityTest, BindThreadInvalidCore) {
    /// TODO: Create a std::thread with a lambda that does nothing.
    auto worker = std::thread([] { /*nothing in here*/ });

    /// TODO: Call bind_thread_to_core(thread, -1).
    bind_thread_to_core(worker, -1);

    /// TODO: Expect an std::runtime_error to be thrown.
    ASSERT_THROW(std::runtime_error());

    /// TODO: Join the thread.
    worker.join();
}