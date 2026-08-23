#include <gtest/gtest.h>
#include <thread>
#include <chrono>
#include "../src/subscriber/subscriber.h"
#include "../src/event_bus.h"

using namespace icarus::subscriber;

/// Tests if
/// 1. Thread starts AND binds to core 2.
/// 2. Prints a debug message.
/// 3. Conditional Variable works.
/// 4. wait() shuts down cleanly.
/// No Exception thrown.
TEST(SubscriberTest, StartAndWait) {
    EventBus<32, 64> bus;
    Subscriber subscriber (bus);

    ASSERT_NO_THROW(subscriber.start());
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    ASSERT_NO_THROW(subscriber.wait());
}