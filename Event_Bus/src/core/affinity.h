#ifndef ICARUS_AFFINITY_H
#define ICARUS_AFFINITY_H

#include <cassert>
#include <thread>
#include <pthread.h>
#include <stdexcept>

namespace icarus::core {

    /// Bind a thread to a specific CPU core.
    /// @param thread Reference to the thread object.
    /// @param core_id The CPU core identifier (0-indexed).
    /// @throws std::runtime_error if pthread_setaffinity_np fails.
    inline void bind_thread_to_core(std::thread& thread, const int core_id) {
        if (core_id < 0)
            throw std::runtime_error("Core ID must be non-negative");

        const auto handle = thread.native_handle();

        cpu_set_t cpu_id; CPU_ZERO(&cpu_id);
        CPU_SET(core_id, &cpu_id);

        if (const int result = pthread_setaffinity_np(handle, sizeof(cpu_id), &cpu_id); result != 0)
            throw std::runtime_error("Failed to bind to core " + std::to_string(core_id));
    }

}  // namespace icarus::core


#endif //ICARUS_AFFINITY_H
