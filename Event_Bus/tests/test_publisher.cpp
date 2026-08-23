#include <gtest/gtest.h>
#include <thread>
#include <chrono>
#include "../src/publisher/publisher.h"
#include "../src/event_bus.h"

using namespace icarus::publisher;

TEST(PublisherTest, StartAndWait) {
    // TODO: Create or mock an EventBus instance.
    EventBus<32, 64> bus;

    // TODO: Instantiate Publisher<EventBus> with the event bus.
    Publisher publisher (bus);

    // TODO: Call start().
    ASSERT_NO_THROW(publisher.start());

    // TODO: Sleep for 100 milliseconds.
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // TODO: Call wait().
    ASSERT_NO_THROW(publisher.wait());
}