#ifndef ICARUS_PUBLISHER_H
#define ICARUS_PUBLISHER_H

#include "../core/affinity.h"
#include "../event_bus.h"

#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>

namespace icarus::publisher {

template <typename EventBus>
class Publisher {
public:
    Publisher() = default;

    /// Parametrized constructor. Stores reference to the event bus.
    /// @param event_bus Reference to the ring buffer event bus.
    explicit Publisher(EventBus& event_bus) : event_bus_(&event_bus) {}

    /// TODO: Delete copy constructor
    /// TODO: Delete copy assignment operator
    Publisher (const Publisher& other) = delete;
    Publisher operator=(Publisher& other) = delete;

    /// TODO: Implement move constructor. Transfer thread ownership and event bus pointer.
    Publisher(Publisher&& other) noexcept {
        worker_ = std::move(other.worker_);
        event_bus_ = std::move(other.event_bus_);
    }

    /// TODO: Implement move assignment. Transfer thread ownership and event bus pointer.
    Publisher& operator=(Publisher&& other) noexcept {
        worker_ = std::move(other.worker_);
        event_bus_ = std::move(other.event_bus_);
    }

    /// TODO: Implement destructor. Call wait() to shut down the worker thread.
    ~Publisher() {
        wait();
    }

    /// Start the producer thread. Pins it to core 1.
    /// TODO: Check if worker is already running. If so, return early.
    /// TODO: Check if event_bus_ is null. If so, throw std::runtime_error.
    /// TODO: Set shutdown_flag_ to false.
    /// TODO: Create a std::jthread with a lambda that calls run().
    void start() {
        if (worker_) return;
        if (event_bus_ == nullptr) throw std::runtime_error("Event bus isn't initialized.");

        shutdown_flag_.store(false, std::memory_order_release);
        worker_ = std::jthread(run());
    }

    /// Signal the worker thread to wake and process pending events.
    /// TODO: Notify the condition variable with notify_one().
    void ready() {
        notify_cv_.notify_one();
    }

    /// Stop the producer thread and wait for it to join.
    /// TODO: Set shutdown_flag_ to true.
    /// TODO: Notify the condition variable to wake the sleeping thread.
    /// TODO: If worker is joinable, join it.
    void wait() {
        shutdown_flag_.store(true, std::memory_order_release);
        notify_cv_.notify_one();
        if (worker_.joinable()) worker_.join();
    }

private:
    /// Worker thread entry point. Binds to core 1 and enters the event loop.
    /// TODO: Call bind_thread_to_core(worker_, 1) to pin to core 1.
    /// TODO: Print a debug message confirming the thread started.
    /// TODO: Enter a loop while shutdown_flag_ is false.
    /// TODO: Lock the notify_mutex_ and wait on notify_cv_.
    /// TODO: Check shutdown_flag_ again. If true, break the loop.
    /// TODO: Placeholder for ingress integration: ingest data and push to ring buffer.
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
