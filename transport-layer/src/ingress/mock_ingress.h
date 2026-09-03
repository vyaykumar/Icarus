#ifndef ICARUS_MOCK_INGRESS_H
#define ICARUS_MOCK_INGRESS_H

#include "ingress_interface.h"
#include <array>

class MockIngress : public IngressSource {
private:
    std::array<uint8_t, 256> buffer;
    uint32_t current_size;
    bool data_available;
    
public:
    MockIngress();
    const uint8_t* get_next_packet() override;
    uint32_t packet_size() override;
    bool has_data() override;
};

#endif // ICARUS_MOCK_INGRESS_H