#ifndef ICARUS_INGRESS_INTERFACE_H
#define ICARUS_INGRESS_INTERFACE_H

#include <cstdint>

class IngressSource {
public:
    virtual ~IngressSource() = default;
    
    virtual const uint8_t* get_next_packet() = 0;
    virtual uint32_t packet_size() = 0;
    virtual bool has_data() = 0;
};

#endif // ICARUS_INGRESS_INTERFACE_H