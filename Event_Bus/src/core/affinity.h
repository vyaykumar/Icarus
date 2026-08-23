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
    template <typename ThreadType>
inline void bind_thread_to_core(ThreadType& thread, int core_id) {
        if (core_id < 0) {
            throw std::runtime_error("Core ID must be non-negative");
        }

        pthread_t native_handle = thread.native_handle();
        cpu_set_t cpu_set;
        CPU_ZERO(&cpu_set);
        CPU_SET(core_id, &cpu_set);

        int result = pthread_setaffinity_np(native_handle, sizeof(cpu_set_t), &cpu_set);
        if (result != 0) {
            throw std::runtime_error("Failed to bind thread to core " + std::to_string(core_id));
        }
    }

}  // namespace icarus::core


#endif //ICARUS_AFFINITY_H
