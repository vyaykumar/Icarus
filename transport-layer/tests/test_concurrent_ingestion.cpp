#include <gtest/gtest.h>

#include "ingress/mock_ingress.h"
#include "ingress/event_director.h"
#include "transport/event_bus.h"
#include "publisher/publisher.h"
#include "subscriber/subscriber.h"

using namespace icarus::subscriber;
using namespace icarus::publisher;

TEST (ConcurrentTest, EventDirector_And_Subscriber_Concurrent) {
    MockIngressSource source {};
    EventBus<8192, 8192> event_bus {};
    Publisher publisher (event_bus);
    Subscriber subscriber (event_bus);

    EventDirector director (source, publisher, event_bus);

    director.start();
    subscriber.start();

    while (director.sequence_count() < 1000)
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

    std::this_thread::sleep_for(std::chrono::milliseconds(100));  // Let subscriber drain

    director.wait();
    subscriber.wait();

    std::cout << "Director processed: " << director.sequence_count() << " events" << std::endl;
    std::cout << "Subscriber consumed: " << subscriber.event_count() << " events" << std::endl;
    EXPECT_EQ(subscriber.event_count(), 1000);
}