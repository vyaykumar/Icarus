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

#include <cstring>

template<typename EventBus>
EventDirector<EventBus>::EventDirector(IngressReceiver& ingress, Publisher<EventBus>& publisher, EventBus& event_bus)
    : ingress_(ingress),
      publisher_(publisher),
      event_bus_(event_bus),
      sequence_counter_(0),
      shutdown_flag_(false) { }

template<typename EventBus>
void EventDirector<EventBus>::start() {
    worker_ = std::jthread([this]() { work_loop_(); });
}

template<typename EventBus>
void EventDirector<EventBus>::wait() {
    shutdown_flag_.store(true, std::memory_order_release);
    worker_.join();
}

template<typename EventBus>
uint64_t EventDirector<EventBus>::sequence_count() const {
    return sequence_counter_;
}

template<typename EventBus>
void EventDirector<EventBus>::work_loop_() {
    while (!shutdown_flag_) {
        auto bytes_read = ingress_.poll(receive_buffer_, 4096);

        if (bytes_read > 0) {
            auto handle = event_bus_.allocate_payload();
            auto payload_ptr = event_bus_.get_payload(handle);

            std::memcpy(payload_ptr, receive_buffer_, bytes_read);

            Event event {
                .nano_stamp = static_cast<uint64_t>(std::chrono::high_resolution_clock::now().time_since_epoch().count()),
                .sequence_number = ++sequence_counter_,
                .payload_size = bytes_read,
                .payload_handle = handle,
                // .reserved =
            };

            event_bus_.push (event);
        }

        std::this_thread::sleep_for(std::chrono::nanoseconds(100));
    }
}

#endif //ICARUS_EVENT_DIRECTOR_H
