#include <gtest/gtest.h>
#include <signal.h>
#include <setjmp.h>
#include "../src/guarded-pool/guarded_memory_pool.h"

static jmp_buf jump_buffer;
static bool segfault_caught = false;

void segfault_handler(int sig) {
    segfault_caught = true;
    longjmp(jump_buffer, 1);
}

TEST (GuardPoolTest, CatchOverflow) {
    signal(SIGSEGV, segfault_handler);

    GuardedMemoryPool<uint8_t, 1024> g_pool;

    for (int i = 0; i < 1024; ++i) {
        uint8_t* ptr = g_pool.get(i);
        *ptr = static_cast<uint8_t>(i % 256);
    }

    if (setjmp(jump_buffer) == 0) {
        uint8_t* overflow_ptr = g_pool.get(1024);
        *overflow_ptr = 42;
    }

    EXPECT_TRUE(segfault_caught);
}