#ifndef ICARUS_MOCK_INGRESS_HPP
#define ICARUS_MOCK_INGRESS_HPP

#include "ingress.h"
#include <vector>
#include <cstdint>

class MockIngressSource : public IIngressReceiver {
private:
    std::vector<std::vector<uint8_t>> pregenerated_frames_;
    size_t current_frame_index_;
    size_t current_offset_;

public:
    MockIngressSource();
    size_t poll(uint8_t* destination_buffer, size_t max_len) override;
};

#endif // ICARUS_MOCK_INGRESS_HPP