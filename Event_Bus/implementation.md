Header file implementations:
+ Event structure: 64 bytes, packed, with generation counter validation
+ RingBuffer: SPSC, lock-free, acquire/release semantics, wrapping with modulo
+ MemoryPool: pre-allocated storage, generation tracking, use-after-free detection
+ EventBus: two buffers (command and response), payload pool, simulation parameters