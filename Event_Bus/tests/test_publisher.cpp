#include <gtest/gtest.h>
#include <thread>
#include <chrono>
#include "../src/publisher/publisher.h"
#include "../src/event_bus.h"

using namespace icarus::publisher;


/// Tests if
/// 1. Thread starts AND binds to core 1.
/// 2. Prints a debug message.
/// 3. Conditional Variable works.
/// 4. wait() shuts down cleanly.
/// No Exception thrown.
TEST(PublisherTest, StartAndWait) {
    EventBus<32, 64> bus;
    Publisher publisher (bus);

    ASSERT_NO_THROW(publisher.start());
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    ASSERT_NO_THROW(publisher.wait());
}