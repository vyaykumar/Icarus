#include <gtest/gtest.h>
#include <thread>
#include "../src/core/affinity.h"

using namespace icarus::core;

TEST(AffinityTest, BindThreadToCore) {
    // TODO: Create an int counter initialized to 0.
    // TODO: Create a std::thread with a lambda that increments the counter 1000 times.
    // TODO: Call bind_thread_to_core(thread, 1).
    // TODO: Assert no exception is thrown.
    // TODO: Join the thread.
    // TODO: Assert counter == 1000.
}

TEST(AffinityTest, BindThreadInvalidCore) {
    // TODO: Create a std::thread with a lambda that does nothing.
    // TODO: Call bind_thread_to_core(thread, -1).
    // TODO: Expect an std::runtime_error to be thrown.
    // TODO: Join the thread.
}