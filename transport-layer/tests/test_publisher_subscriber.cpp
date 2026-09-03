#include <gtest/gtest.h>
#include <chrono>
#include <thread>
#include "../src/transport/event_bus.h"
#include "../src/publisher/publisher.h"
#include "../src/subscriber/subscriber.h"

using namespace icarus::publisher;
using namespace icarus::subscriber;

TEST(PublisherSubscriberTest, Integration) {
    // TODO: Create EventBus<32, 64> instance.
    EventBus<32, 64> bus;

    // TODO: Create Publisher with event bus.
    Publisher publisher (bus);

    // TODO: Create Subscriber with event bus.
    Subscriber subscriber (bus);

    // TODO: Call publisher.start().
    publisher.start();

    // TODO: Call subscriber.start().
    subscriber.start();

    // TODO: Call publisher.ready() to signal work.
    publisher.ready();

    // TODO: Sleep 100 milliseconds.
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // TODO: Call publisher.wait() and subscriber.wait().
    publisher.wait();
    subscriber.wait();

    // TODO: Assert no exceptions.
}