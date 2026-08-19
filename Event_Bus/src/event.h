#ifndef ICARUS_EVENT_H
#define ICARUS_EVENT_H

#include <cstdint>

#pragma pack(1)
struct Event {
    uint64_t nano_stamp {};
    uint64_t sequence_number {};
    uint64_t payload_size {};
    uint8_t reserved[40] {};
};
#pragma pack()

static_assert(sizeof(Event) == 64, "Event must be 64 bytes.");

#endif //ICARUS_EVENT_H
