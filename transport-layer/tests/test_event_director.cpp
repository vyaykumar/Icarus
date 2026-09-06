#include <gtest/gtest.h>

#include "ingress/mock_ingress.h"
#include "ingress/event_director.h"
#include "transport/event_bus.h"
#include "publisher/publisher.h"
#include "subscriber/subscriber.h"

using namespace icarus::publisher;
using namespace icarus::subscriber;

TEST (EventDirectorTest, ThousandMessageIngress) {
    MockIngressSource source;

    EventBus<4096, 8192> event_bus {};

    Publisher publisher(event_bus);

    EventDirector director (source, publisher, event_bus);

    director.start();

    while (director.sequence_count() < 1000)
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

    director.wait();

    uint64_t expected_sequence = 1;
    int events_popped = 0;

    Event event;
    while (event_bus.pop(event)) {
        EXPECT_EQ(event.sequence_number, expected_sequence);
        EXPECT_EQ(event.payload_size, 128);

        expected_sequence++;
        events_popped++;
    }

    EXPECT_EQ(events_popped, 1000);
}
