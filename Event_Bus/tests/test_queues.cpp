#include <gtest/gtest.h>
#include "../src/ring_buffer.h"
#include "../src/memory_pool.h"
#include "../src/event_bus.h"

TEST(RingBufferTest, PushAndPop) {
    RingBuffer<16> buffer;
    Event event = {};
    event.nano_stamp = 12345;

    bool pushed = buffer.push(event);
    ASSERT_TRUE(pushed);

    Event received = {};
    bool popped = buffer.pop(received);
    ASSERT_TRUE(popped);
}

TEST(RingBufferTest, EmptyBuffer) {
    RingBuffer<16> buffer;

    Event event = {};
    bool popped = buffer.pop(event);
    ASSERT_FALSE(popped);
}

TEST(RingBufferTest, FullBuffer) {
    RingBuffer<2> buffer;
    constexpr Event event = {};

    ASSERT_TRUE(buffer.push(event));
    ASSERT_FALSE(buffer.push(event));
}

TEST(RingBufferTest, Wrapping) {
    RingBuffer<2> buffer;
    constexpr Event event1 = {.nano_stamp = 100};
    constexpr Event event2 = {.nano_stamp = 200};
    constexpr Event event3 = {.nano_stamp = 300};

    Event temp = {};
    buffer.push(event1);
    buffer.push(event2);

    buffer.pop(temp);
    buffer.push(event3);
    buffer.pop(temp);
    buffer.pop(temp);
    
    ASSERT_TRUE(buffer.is_empty());
}

TEST(MemoryPoolTest, AllocateAndDeallocate) {
    MemoryPool<uint8_t, 16> pool{};

    const Handle h1 = pool.allocate();
    ASSERT_NE(h1.index, UINT32_MAX);

    uint8_t* ptr = pool.get(h1);
    ASSERT_NE(ptr, nullptr);

    pool.deallocate(h1);

    // Allocate again to increment generation
    Handle h2 = pool.allocate();
    ASSERT_EQ(h2.index, h1.index); // Same slot reused

    uint8_t* ptr_after = pool.get(h1);
    ASSERT_EQ(ptr_after, nullptr); // Old handle rejected
}

TEST(MemoryPoolTest, GenerationValidation) {
    MemoryPool<uint8_t, 16> pool{};

    auto [index, generation] = pool.allocate();
    const Handle fake_handle = {.index = index, .generation = generation + 1};

    uint8_t* ptr = pool.get(fake_handle);
    ASSERT_EQ(ptr, nullptr);
}

TEST(EventBusTest, SendAndReceive) {
    EventBus<16, 256> bus;
    constexpr Event event{.nano_stamp = 12345, .sequence_number = 1};

    bus.send(event);

    Event received{};
    bool success = bus.command_buffer.pop(received);
    ASSERT_TRUE(success);
    ASSERT_EQ(received.nano_stamp, 12345);
    ASSERT_EQ(received.sequence_number, 1);
}