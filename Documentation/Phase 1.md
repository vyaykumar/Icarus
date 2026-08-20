### Phase 1 Objectives

The first phase builds the core transport abstraction. The system requires an event bus. The event bus imitates a network fabric. The code relies on the lock-free communication.

- [ ] If you write the C++26 code, deploy the native hazard pointers.
- [ ] If you allocate the buffers, apply the `std::inplace_vector` container.
- [ ] If you construct the object pools, use a `constexpr` placement new operation.
- [ ] If you build the event bus, construct the fixed-size ring buffers.
- [ ] If you process the consumer reads, implement the acquire semantics.
- [ ] If you process the producer writes, implement the release semantics.
- [ ] If you define the event, create a 64-byte event structure.
- [ ] If you pack the structure, include a nanosecond timestamp and a sequence number.
- [ ] If you complete the structure, include a payload pointer and a payload size.
- [ ] If you manage the memory, construct the pools with the generation counters.
- [ ] If you allocate the critical buffers, enclose them with the guard pages.
- [ ] If you simulate the network, add the latency, the jitter, and the packet loss.
- [ ] If you route the large payloads, implement a kernel zero-copy path.

### Phase 1 Tests

The test phase checks the code correctness and the execution speed. Each subsystem requires the strict unit tests.

- [ ] If you test the queues, write the unit tests for the boundary cases.
- [ ] If you test the memory, verify the overflow detection on the arenas.
- [ ] If you test the event bus, measure the event delivery latency.
- [ ] If you evaluate the speed, reach a latency time under 500 nanoseconds.
- [ ] If you evaluate the throughput, push one million events per second.
- [ ] If you run the heavy load, ensure the system creates zero errors.