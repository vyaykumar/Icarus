#ifndef ICARUS_EVENT_H
#define ICARUS_EVENT_H

#include <cstdint>

struct Event {
    uint64_t nano_stamp;
    uint64_t sequence_number;
    void* payload_pointer;
    uint64_t payload_size;
    uint8_t reserved[32];
};

static_assert(sizeof(Event) == 64, "Event must be 64 bytes.");

#endif //ICARUS_EVENT_H
