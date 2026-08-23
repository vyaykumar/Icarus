#ifndef ICARUS_PUBLISHER_H
#define ICARUS_PUBLISHER_H

#include "../core/affinity.h"
#include "../event_bus.h"

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
    explicit Publisher(EventBus& event_bus) : event_bus_(&event_bus) {}

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
        if (event_bus_ == nullptr) throw std::runtime_error("Event bus isn't initialized.");

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
        std::cout << "Debug: Thread started.\n";
        while (!shutdown_flag_.load(std::memory_order::relaxed)) {
            std::unique_lock lock (notify_mutex_);
            notify_cv_.wait(lock);

            if (shutdown_flag_.load(std::memory_order::relaxed)) break;

            //ingress placeholder.
            std::this_thread::sleep_for(std::chrono::nanoseconds(50));
        }
    }

    EventBus* event_bus_ = nullptr;
    std::jthread worker_;
    std::atomic<bool> shutdown_flag_{false};
    std::mutex notify_mutex_;
    std::condition_variable notify_cv_;
};

}  // namespace icarus::publisher


#endif
