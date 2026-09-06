#ifndef ICARUS_SUBSCRIBER_H
#define ICARUS_SUBSCRIBER_H

#include "../core/affinity.h"
#include "../transport/event_bus.h"

#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <iostream>

namespace icarus::subscriber {

template <typename EventBus>
class Subscriber {
public:
    Subscriber() = default;

    /// Parametrized constructor. Stores reference to the event bus.
    /// @param event_bus Reference to the ring buffer event bus.
    explicit Subscriber(EventBus& event_bus) : event_bus_(event_bus) {}

    Subscriber (const Subscriber& other) = delete;
    Subscriber& operator=(const Subscriber& other) = delete;

    Subscriber(Subscriber&& other) noexcept {
        worker_ = std::move(other.worker_);
        event_bus_ = other.event_bus_;
        other.event_bus_ = nullptr;
    }

    Subscriber& operator=(Subscriber&& other) noexcept {
        worker_ = std::move(other.worker_);
        event_bus_ = other.event_bus_;
        other.event_bus_ = nullptr;
    }

    ~Subscriber() {
        wait();
    }

    /// Start the producer thread. Pins it to core 1.
    void start() {
        if (worker_.joinable()) return;

        shutdown_flag_.store(false, std::memory_order_release);
        worker_ = std::jthread([this] { run(); });
    }

    /// Signal the worker thread to wake and process pending events.
    void ready() {
        notify_cv_.notify_one();
    }

    /// Stop the producer thread and wait for it to join.
    void wait() {
        shutdown_flag_.store(true, std::memory_order_release);
        notify_cv_.notify_one();
        if (worker_.joinable()) worker_.join();
    }

    uint64_t event_count() const {
        return event_count_;
    }

private:
    void run() {
        core::bind_thread_to_core(worker_, 2);
        std::cout << "Debug: Subscriber thread bound to Core_2.\n";

        while (!shutdown_flag_.load(std::memory_order::relaxed)) {
            Event event;
            if (event_bus_.pop(event)) {
                ++event_count_;

                Handle handle = event.payload_handle;

                uint8_t* payload = event_bus_.get_payload(handle);
                if (!payload) {
                    std::cerr << "Failed to resolve payload.\n";
                    continue;
                }

                auto* data = reinterpret_cast<uint64_t*> (payload);
                if (data) {
                    uint64_t value = *data;
                    std::cout << "Subscriber received payload: " << value << "\n";
                }

                event_bus_.deallocate_payload(handle);
            }
            else {
                std::this_thread::sleep_for(std::chrono::nanoseconds(100));
            }
        }
    }

    EventBus& event_bus_ {};
    std::jthread worker_;
    std::atomic<bool> shutdown_flag_{false};
    std::mutex notify_mutex_;
    std::condition_variable notify_cv_;
    std::atomic<uint64_t> event_count_ {};
};

}  // namespace icarus::subscriber

#endif //ICARUS_SUBSCRIBER_H
