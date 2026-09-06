#include <gtest/gtest.h>
#include <signal.h>
#include <setjmp.h>
#include "guarded-pool/guarded_memory_pool.h"

static jmp_buf jump_buffer;
static bool segfault_caught = false;

void segfault_handler(int sig) {
    segfault_caught = true;
    longjmp(jump_buffer, 1);
}

TEST (GuardPoolTest, CatchOverflow) {
    /// TODO: Install SIGSEGV signal handler
    signal(SIGSEGV, segfault_handler);

    /// TODO: Create GuardedMemoryPool<uint8_t, 1024>
    GuardedMemoryPool<uint8_t, 1024> g_pool;

    /// TODO: Write valid data to slots 0-1023 (should succeed)
    for (int i = 0; i < 1024; ++i) {
        uint8_t* ptr = g_pool.get(i);
        *ptr = static_cast<uint8_t>(i % 256);
    }

    /// TODO: Set jump point for signal handler
    if (setjmp(jump_buffer) == 0) {
        /// TODO: Attempt to write to slot 1024 (should trigger SIGSEGV)
        uint8_t* overflow_ptr = g_pool.get(1024);
        *overflow_ptr = 42;
    }

    /// TODO: Verify segfault was caught
    EXPECT_TRUE(segfault_caught);
}