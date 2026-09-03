#include "mock_ingress.h"

MockIngress::MockIngress() : current_size(64), data_available(true) {
    for (int i = 0; i < 64; ++i) {
        buffer[i] = static_cast<uint8_t>(i % 256);
    }
}

const uint8_t* MockIngress::get_next_packet() {
    return buffer.data();
}

uint32_t MockIngress::packet_size() {
    return current_size;
}

bool MockIngress::has_data() {
    return data_available;
}