#ifndef ICARUS_EVENT_DIRECTOR_H
#define ICARUS_EVENT_DIRECTOR_H

#include "ingress.h"
#include "publisher/publisher.h"
#include <cstdint>
#include <atomic>
#include <thread>

using namespace icarus::publisher;

template<typename EventBus>
class EventDirector {
public:
    EventDirector(IngressReceiver& ingress, Publisher<EventBus>& publisher, EventBus& event_bus);
    void start();
    void wait();
    uint64_t sequence_count() const;

private:
    IngressReceiver& ingress_;
    Publisher<EventBus>& publisher_;
    EventBus& event_bus_;
    uint8_t receive_buffer_[4096] {};
    uint64_t sequence_counter_;
    std::atomic<bool> shutdown_flag_;
    std::jthread worker_;

    void work_loop_ ();
};

#endif //ICARUS_EVENT_DIRECTOR_H
