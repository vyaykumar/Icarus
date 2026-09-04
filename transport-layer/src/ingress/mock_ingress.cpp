#include "mock_ingress.h"

#include <chrono>
#include <cstring>

MockIngressSource::MockIngressSource()
    : current_frame_index_(0), current_offset_(0) {

    for (int i = 0; i < 1000; ++i) {
        std::vector<uint8_t> frame(128, 0);

        uint64_t seq = i + 1;
        std::memcpy(&frame[0], &seq, sizeof(uint64_t));

        uint64_t ts = i * 1000;
        std::memcpy(&frame[8], &ts, sizeof(uint64_t));

        uint32_t order_id {};
        std::memcpy(&frame[16], &order_id, sizeof(uint32_t));

        bool side {};
        std::memcpy(&frame[20], &side, sizeof(bool));

        // 7Bytes padded.

        uint32_t price_scaled {};
        std::memcpy(&frame[28], &price_scaled, sizeof(uint32_t));

        uint32_t quantity {};
        std::memcpy(&frame[32], &quantity, sizeof(uint32_t));

        uint64_t symbol_hash {};
        std::memcpy(&frame[36], &symbol_hash, sizeof(uint64_t));

        // 84Bytes padded.

        pregenerated_frames_.push_back(frame);
    }
}
