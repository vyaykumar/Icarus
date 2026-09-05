#include "event_director.h"

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

            const auto now = std::chrono::high_resolution_clock::now();
            Event event {
                .nano_stamp = std::chrono::high_resolution_clock::now().time_since_epoch().count(),
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