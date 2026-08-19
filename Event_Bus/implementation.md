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
- Methods:
    - `bool push(const Event&)`: write event, release semantics, returns false if full
    - `bool pop(Event&)`: read event, acquire semantics, returns false if empty
    - `bool is_empty() const`: check if buffer has no events
    - `bool is_full() const`: check if buffer cannot accept events
    - `uint32_t size() const`: return count of events in buffer
- Wrapping with modulo `%` Capacity on both indices
- Full check: `next_write == read_idx`
- Empty check: `write_idx == read_idx`

## MemoryPool (memory_pool.h)

- Struct: `Handle` with index and generation fields
- Template: `MemoryPool<typename T, uint32_t Capacity>`
- `std::array<T, Capacity> storage`: pre-allocated objects
- `std::array<uint32_t, Capacity> generations`: version counter per slot
- `std::array<bool, Capacity> available`: free slot tracking
- Methods:
    - `Handle allocate()`: find available slot, increment generation, mark unavailable, return Handle
    - `void deallocate(Handle)`: validate generation, mark slot available
    - `T* get(Handle)`: validate generation, return pointer to object or nullptr
- Generation validation catches use-after-free

## EventBus (event_bus.h)

- Template: `EventBus<uint32_t BufferCapacity, uint32_t PayloadCapacity>`
- `RingBuffer<BufferCapacity> command_buffer`: inbound events
- `RingBuffer<BufferCapacity> response_buffer`: outbound events
- `MemoryPool<uint8_t, PayloadCapacity> payload_pool`: large payload storage
- `uint32_t latency_ns`: fixed delay simulation
- `uint32_t jitter_ns`: random delay variation
- `float packet_loss_rate`: drop probability (0.0 to 1.0)
- Methods:
    - `void send(const Event&)`: write event to command buffer
    - `bool receive(Event&)`: read event from response buffer, returns false if empty
    - `Handle allocate_payload()`: allocate slot in payload pool, return Handle
    - `uint8_t* get_payload(Handle)`: retrieve payload pointer, return nullptr if invalid Handle
- Payload size stored in `event.payload_size`
- Zero-copy threshold configured but not yet implemented

## Next Steps

- Add latency, jitter, and packet loss simulation to send/receive
- Implement zero-copy path for payloads above threshold
- Add guard pages for buffer overflow detection
- Write unit tests for boundary cases and heavy load