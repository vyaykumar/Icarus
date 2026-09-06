# Project Icarus

High-performance trading backtesting infrastructure in C++26, targeting sub-microsecond event latency through lock-free concurrency, CPU affinity isolation, and hardware-enforced memory safety.

## Overview

This project solves no new problems. 
It is simply an evolution of scope and features from the previous project of Task-Dispatch, wishing to further my understanding into the domains of High-Performance Computing, and Parallel Programming, all while being constrained to a CPU. (I just lack a GPU, hence the CPU restraint).

This project implements a deterministic event-driven pipeline for replaying and testing trading strategies against historical market data. 
The focus is on understanding and implementing low-latency systems patterns: lock-free queues, CPU core isolation, memory pools, and hardware page protection.

## Current Status

**Phases 1–3: Complete**
- Event bus with lock-free SPSC ring buffer.
- CPU affinity thread binding and Publisher/Subscriber integration.
- Modular ingress pipeline with mock data source and hardware-guarded memory pool.

**Phase 4: In Progress**
- Order matching engine.

**Phases 5–7: Deferred**
- Position and risk management.
- Strategy backtester with two hardcoded strategies.
- Documentation and code polish.

## Build & Run

### Prerequisites

- C++26 compiler (GCC 13+ or Clang 17+)
- CMake 3.20+
- Google Test (installed from source, optional for tests)
- Linux/WSL2 with POSIX threading

### Build

```bash
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
```

### Run Tests (Not yet consolidated)

```bash
cd build
ctest --output-on-failure
```

Or run individual test suites:

```bash
./tests/phase1_tests
./tests/phase2_tests
./tests/phase3_tests
```

### Benchmarks

```bash
./benchmarks/ring_buffer_bench      # Phase 1: 13.6M events/sec, 3.7µs median
./benchmarks/ingress_bench          # Phase 3: 94.5K events/sec, 24ns median
```

## Architecture

### High-Level Data Flow

```
MockIngressSource (core 1)
        ↓
    [poll raw bytes]
        ↓
   EventDirector
   (allocate payload, copy bytes, wrap Event)
        ↓
 RingBuffer (8192 slots, 64 bytes each)
        ↓
 Subscriber (core 2)
   [future: MatchingEngine]
        ↓
  Trade Events / Positions
```

### Core Components

| Component | Phase | Purpose | Notes |
|-----------|-------|---------|-------|
| `Event` struct | 1 | 64-byte packed event (timestamp, sequence, payload handle, embedded 32B buffer) | `#pragma pack(1)` for tight layout |
| `RingBuffer<Capacity>` | 1 | Lock-free SPSC circular buffer | Power-of-two capacity, bitwise AND masking, acquire/release semantics |
| `MemoryPool<T, Capacity>` | 1 | Generation-based payload storage | Detects use-after-free via generation validation |
| `FreeListPool<T, Capacity>` | 3 | O(1) allocation via LIFO stack | Replaces linear-search pool, eliminates P99 spikes |
| `Publisher<EventBus>` | 2 | Test harness that allocates and pushes events | Pinned to core 1, graceful shutdown |
| `Subscriber<EventBus>` | 2 | Test harness that pops and processes events | Pinned to core 2, validates memory pool integration |
| `IngressReceiver` | 3 | Abstract interface for data sources | Pure virtual `poll(buffer, max_len)` |
| `MockIngressSource` | 3 | Deterministic synthetic order frames | 1000 × 128-byte frames, reproducible |
| `EventDirector<EventBus>` | 3 | Ingress orchestrator (poll → allocate → copy → push) | Busy-poll with `_mm_pause()`, sub-microsecond latency |
| `GuardedMemoryPool<T, Capacity>` | 3 | Page-aligned mmap + mprotect guard page | Hardware bounds checking via SIGSEGV, zero cost |

## Performance Results

### Phase 1: Ring Buffer (1M events, 65KB buffer)

| Metric | Value |
|--------|-------|
| P50 (median) latency | 3.7 µs |
| P95 latency | 69 µs |
| P99 latency | 95 µs |
| Minimum latency | 26 ns |
| Throughput | 13.6M events/sec |
| Target | 1M events/sec (exceeded by 13×) |

**Note:** Ring buffer in isolation. End-to-end latency (ingress → matching → output) is measured in Phase 4+.

### Phase 3: Ingress Pipeline (optimised)

| Metric | Value | Notes |
|--------|-------|-------|
| P50 latency | 24 ns | After busy-poll + FreeListPool |
| P95 latency | 25 ns | Highly deterministic |
| P99 latency | 35 ns | Sub-microsecond guaranteed |
| Throughput | 94.5K events/sec | WSL2-constrained; bare metal ~1M/sec |
| Memory allocation | O(1) | Free-list stack, 2–5ns per alloc |
| Guard page overhead | 0 ns | Hardware-enforced, zero cost |

**Optimizations:**
- Busy-poll with `_mm_pause()`: eliminated OS scheduler latency (−90% P99).
- FreeListPool: O(1) allocation replaces O(N) search (−41% P50).
- Pre-allocated stack buffer: no heap allocation in critical path.

### Phase 4: Matching Engine (in progress)

- Target latency: <10µs per order.
- Target throughput: 100K+ orders/sec.

## CPU Core Binding

All threads pinned to dedicated cores to eliminate context-switching overhead:

- **Core 0:** Reserved for OS interrupts and background tasks.
- **Core 1:** EventDirector (ingress producer).
- **Core 2:** Subscriber (test consumer; will be replaced by MatchingEngine).
- **Cores 3+:** Available for future pipeline stages (execution, position manager, etc.).

Binding via `pthread_setaffinity_np()` at thread startup.

## Hardware Features

### Lock-Free Concurrency

- SPSC ring buffer with acquire/release semantics (std::memory_order).
- No mutexes, no CAS loops in critical path.
- Index wrapping via bitwise AND (single CPU cycle).

### CPU Affinity Isolation

- Threads pinned to dedicated cores.
- Eliminates cross-core cache coherence traffic.
- Bounds tail latency (P99 under 95µs for Phase 1).

### Hardware Page Protection

- GuardedMemoryPool uses `mmap` + `mprotect(PROT_NONE)`.
- Any write past valid pool boundary triggers SIGSEGV (hardware MMU).
- Zero runtime overhead during normal operation.
- Catches buffer overruns that software bounds checks would miss.

### Memory Pool with Generation Validation

- Each slot has a generation counter.
- Allocation increments generation, returns (index, generation) pair.
- Deallocation validates generation before freeing.
- Detects use-after-free at runtime without tagging pointers.

## Project Structure

### Transport Layer (Phase 1-3) 
```
.
├── CMakeLists.txt
├── implementation.md
├── src/
│   ├── transport/
│   │   ├── event.h                   # 64-byte Event struct
│   │   ├── event_bus.h               # Ring buffer + memory pool container
│   │   ├── ring_buffer.h             # Lock-free SPSC queue
│   │   └── memory_pool.h             # Generation-based pool
│   ├── memory-pool/
│   │   └── free_list_pool.h          # O(1) allocation via stack
│   ├── guarded-pool/
│   │   └── guarded_memory_pool.h     # mmap + mprotect
│   ├── core/
│   │   └── affinity.h                # CPU binding
│   ├── publisher/
│   │   └── publisher.h               # Test producer
│   ├── subscriber/
│   │   └── subscriber.h              # Test consumer
│   └── ingress/
│       ├── ingress.h                 # Abstract interface
│       ├── mock_ingress.h            # Deterministic mock source
│       └── event_director.h          # Ingestion orchestrator
└── tests/
    ├── latency_utils.h
    ├── test_affinity.cpp
    ├── test_concurrent_ingestion.cpp
    ├── test_event_director.cpp
    ├── test_guarded_pool.cpp
    ├── test_ingestion_benchmark.cpp
    ├── test_performance.cpp
    ├── test_publisher_subscriber.cpp
    └── test_queues.cpp
```

## Key Design Decisions

### Why Lock-Free?

Lock-free queues avoid mutex contention and OS scheduler latency. With CPU affinity, producer and consumer run on separate cores with zero contention. Acquire/release semantics ensure visibility without explicit synchronisation.

### Why Separate Ingress Interface?

The abstract `IngressReceiver` decouples the data source (CSV file, socket, RNG, broker API) from the event pipeline. Enables testing with mock data (deterministic replay) without real network I/O.

### Why Memory Pools?

Pre-allocated pools eliminate dynamic allocation in the critical path. Generation-based handles catch use-after-free errors. FreeListPool provides O(1) allocation, eliminating tail-latency spikes from linear search.

### Why Hardware Guard Pages?

Software bounds checks (conditional branches) add latency. Hardware page protection via `mprotect` detects buffer overruns with zero cost during normal operation. Failure mode is a segmentation fault (immediate crash), not silent corruption.

### Why Busy-Poll?

On isolated cores, busy-polling with `_mm_pause()` eliminates OS scheduler latency from nanosleep syscalls. CPU hint prevents pipeline thrashing. Only used in tight ingestion loops; tests and initialization use OS sleep.

<!-- QUESTION: Are there design decisions you'd change if starting over? -->

## What's Not Included (Intentionally)

- **Logging:** Printf/logging in hot path introduces latency. Use atomic counters for metrics.
- **Configuration hot-reload:** Fix parameters at startup. No dynamic reconfiguration during trading.
- **Multithreaded matching:** Single matching thread simplifies logic. Multithreaded matching is a Phase 5+ extension.
- **Real network:** Mock data source is sufficient for backtesting. Real broker APIs tested separately.
- **Compression/encryption:** Teaching project, not production. No data compression or TLS.

## Testing Strategy

### Unit Tests

- Ring buffer: push/pop correctness, full/empty, size
- Memory pool: allocate/deallocate, generation validation, use-after-free detection
- Affinity: thread binding verification (via /proc/self/status)
- Ingress: mock source determinism, EventDirector pipeline, concurrent producer/consumer
- Guard pages: valid writes succeed, out-of-bounds writes trigger SIGSEGV

### Integration Tests

- End-to-end: EventDirector → RingBuffer → Subscriber (validate order, no events lost).
- Concurrent: Publisher + Subscriber running simultaneously, verify no data races.

### Benchmarks

- Ring buffer: latency percentiles (P50, P95, P99), throughput.
- Ingress: poll-to-push latency, allocation cost, busy-poll efficiency.
- Memory pool: allocation/deallocation latency (O(1) vs O(N)).

## For Interviewers

**Core contributions:**
1. Lock-free SPSC ring buffer with acquire/release semantics
2. CPU affinity isolation eliminating context-switching overhead
3. Generation-based memory pool detecting use-after-free
4. O(1) allocation via free-list stack (eliminating P99 tail latency)
5. Hardware page protection via mprotect (zero-cost bounds checking)
6. Abstract ingress interface (pluggable data sources)
7. Busy-poll with _mm_pause() (eliminating OS scheduler latency)

**Key talking points:**
- Sub-microsecond event latency achieved through lock-free concurrency and CPU isolation
- Hardware bounds checking (page protection) is faster than software bounds checking (conditional branches)
- Generation-based handles catch subtle bugs (use-after-free) that memory tagging would miss
- Deterministic replay via mock data source enables reproducible testing
- Free-list allocation eliminates latency spikes; P99 tail latency is more important than average
- Trade-offs: sacrificed ease-of-programming (no high-level abstractions) for latency predictability

## Next Steps

**Phase 4: Matching Engine**
- Order book data structure (bid/ask trees).
- Order matching logic (limit, market, IOC).
- Trade event generation.
- Bounds checking and invariant validation.
- 20+ unit tests covering edge cases.

**Phase 5: Position & Risk**
- Fixed-point arithmetic for P&L (no floating-point money).
- Position tracking per symbol.
- Risk limit enforcement (max position, max loss).

**Phase 6: Backtester & Strategies**
- Two hardcoded strategies (moving average, mean reversion).
- Shell script runner (load data, replay, execute, output P&L).

**Phase 7: Documentation & Polish**
- Organize files into appropriate directories.
- Consolidate tests into a single global directory, maybe even providing run configurations.
- Professional README (this file).
- Inline comments on hard parts.
- Benchmark results document.
- Code review for interview readiness.

## Glossary

- **Latency:** Time from event arrival to completion (ingestion, processing, output).
- **Throughput:** Events processed per second.
- **P50, P95, P99:** 50th, 95th, 99th percentile latency (median and tail).
- **SPSC:** Single-producer, single-consumer queue.
- **Acquire/release semantics:** Memory ordering guarantees for cross-thread visibility (std::memory_order).
- **CPU affinity:** Pinning a thread to a specific processor core.
- **SIGSEGV:** Segmentation fault signal (raised by OS when MMU detects invalid memory access).
- **mmap/mprotect:** Memory mapping and protection syscalls.
- **_mm_pause():** CPU hint instruction (x86 PAUSE) to prevent pipeline thrashing during busy-wait.

---

**Last updated:** 6th September 2026

**Phases complete:** 3 of 7  
