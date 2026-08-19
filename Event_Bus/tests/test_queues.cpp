#include <gtest/gtest.h>
#include "../src/ring_buffer.h"

TEST(RingBufferTest, PushAndPop) {
    RingBuffer<16> buffer;
    Event event = {};
    event.nano_stamp = 12345;
    
    bool pushed = buffer.push(event);
    ASSERT_TRUE(pushed);
    
    bool popped = buffer.pop();
    ASSERT_TRUE(popped);
}

TEST(RingBufferTest, EmptyBuffer) {
    RingBuffer<16> buffer;
    
    bool popped = buffer.pop();
    ASSERT_FALSE(popped);
}

TEST(RingBufferTest, FullBuffer) {
    RingBuffer<2> buffer;
    Event event = {};
    
    ASSERT_TRUE(buffer.push(event));
    ASSERT_FALSE(buffer.push(event));
}

TEST(RingBufferTest, Wrapping) {
    RingBuffer<2> buffer;
    Event event1 = {.nano_stamp = 100};
    Event event2 = {.nano_stamp = 200};
    Event event3 = {.nano_stamp = 300};
    
    buffer.push(event1);
    buffer.push(event2);
    
    buffer.pop();
    buffer.push(event3);
    buffer.pop();
    buffer.pop();
    
    ASSERT_TRUE(buffer.is_empty());
}