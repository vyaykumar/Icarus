# Event Bus Phase 1 Implementation Notes

## Event Structure (event.h)

- 64 bytes, `#pragma pack(1)` for tight packing
- `uint64_t timestamp_ns`: nanosecond timestamp
- `uint64_t sequence_number`: event identifier
- `void* payload_pointer`: reference to payload data
- `uint64_t payload_size`: payload byte count
- `uint8_t reserved[32]`: padding to 64 bytes
- `static_assert(sizeof(Event) == 64)`

## RingBuffer (ring_buffer.h)

- Template: `RingBuffer<uint32_t Capacity>`
- `Event buffer[Capacity]`: fixed-size circular buffer
- `std::atomic<uint32_t> write_index`: producer position
- `std::atomic<uint32_t> read_index`: consumer position
- `push(const Event&)`: returns bool, uses `memory_order_release`
- `pop(Event&)`: returns bool, uses `memory_order_acquire`
- Wrapping with modulo `%` Capacity on both indices
- Full check: `next_write == read_idx`
- Empty check: `write_idx == read_idx`

## MemoryPool (memory_pool.h)

- Template: `MemoryPool<typename T, uint32_t Capacity>`
- `std::array<T, Capacity> storage`: pre-allocated objects
- `std::array<uint32_t, Capacity> generations`: version counter per slot
- `std::array<bool, Capacity> available`: free slot tracking
- Handle struct: index and generation pair
- Allocate increments generation on reuse
- Deallocate marks slot available
- Access validates generation matches

## EventBus (event_bus.h)

- Template: `EventBus<uint32_t BufferCapacity, uint32_t PayloadCapacity>`
- `RingBuffer<BufferCapacity> command_buffer`: inbound events
- `RingBuffer<BufferCapacity> response_buffer`: outbound events
- `MemoryPool<uint8_t, PayloadCapacity> payload_pool`: large payload storage
- `uint32_t latency_ns`: fixed delay simulation
- `uint32_t jitter_ns`: random delay variation
- `float packet_loss_rate`: drop probability (0.0 to 1.0)

## Next Steps

- Implement allocate/deallocate in MemoryPool
- Implement send/receive in EventBus
- Add guard pages for buffer overflow detection
- Implement zero-copy path for large payloads