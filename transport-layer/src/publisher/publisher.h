#ifndef ICARUS_PUBLISHER_H
#define ICARUS_PUBLISHER_H

#include "../core/affinity.h"
#include "../transport/event_bus.h"

#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <iostream>

namespace icarus::publisher {

template <typename EventBus>
class Publisher {
public:
    Publisher() = default;

    /// Parametrized constructor. Stores reference to the event bus.
    /// @param event_bus Reference to the ring buffer event bus.
    explicit Publisher(EventBus& event_bus) : event_bus_(event_bus) {}

    Publisher (const Publisher& other) = delete;
    Publisher operator=(Publisher& other) = delete;

    Publisher(Publisher&& other) noexcept {
        worker_ = std::move(other.worker_);
        event_bus_ = other.event_bus_;
        other.event_bus_ = nullptr;
    }

    Publisher& operator=(Publisher&& other) noexcept {
        worker_ = std::move(other.worker_);
        event_bus_ = other.event_bus_;
        other.event_bus_ = nullptr;
    }

    ~Publisher() {
        wait();
    }

    /// Start the producer thread. Pins it to core 1.
    void start() {
        if (worker_.joinable()) return;
        if (&event_bus_ == nullptr) throw std::runtime_error("Event bus isn't initialized.");

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

private:
    /// Worker thread entry point. Binds to core 1 and enters the event loop.
    void run() {
        core::bind_thread_to_core(worker_, 1);
        std::cout << "Debug: Publisher thread bound to Core_1.\n";

        while (!shutdown_flag_.load(std::memory_order::relaxed)) {
            std::unique_lock lock (notify_mutex_);
            notify_cv_.wait(lock, [&] {
                return shutdown_flag_.load(std::memory_order::relaxed);
            });

            if (shutdown_flag_.load(std::memory_order::relaxed)) break;

            //ingress placeholder.
            // std::this_thread::sleep_for(std::chrono::nanoseconds(50));

            Handle handle = event_bus_.allocate_payload();

            if (handle.index == UINT32_MAX) {
                std::cerr << "Payload Allocation Failed. Pool Exhausted.\n";
                continue;
            }

            uint8_t* payload = event_bus_.get_payload(handle);
            auto* data = reinterpret_cast<uint64_t*> (payload);
            static uint64_t counter = 0;
            if (data) *data = counter++;

            const auto now = std::chrono::high_resolution_clock::now().time_since_epoch().count();
            Event event {
                .nano_stamp = static_cast<uint64_t>(now),
                .sequence_number = counter,
                .payload_size = sizeof(uint64_t),
                .payload_handle = handle,
                .reserved = {}
            };

            event_bus_.push(event);

            // TODO: If allocation fails, log an error and continue.

        }
    }

    EventBus& event_bus_ {};
    std::jthread worker_;
    std::atomic<bool> shutdown_flag_{false};
    std::mutex notify_mutex_;
    std::condition_variable notify_cv_;
};

}  // namespace icarus::publisher


#endif
