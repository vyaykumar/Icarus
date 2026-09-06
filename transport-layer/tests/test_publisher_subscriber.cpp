#include <gtest/gtest.h>
#include <chrono>
#include <thread>
#include "../src/transport/event_bus.h"
#include "../src/publisher/publisher.h"
#include "../src/subscriber/subscriber.h"

using namespace icarus::publisher;
using namespace icarus::subscriber;

TEST(PublisherSubscriberTest, Integration) {
    EventBus<32, 64> bus;

    Publisher publisher (bus);
    Subscriber subscriber (bus);

    publisher.start();
    subscriber.start();

    publisher.ready();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    publisher.wait();
    subscriber.wait();

    // TODO: Assert no exceptions.
}