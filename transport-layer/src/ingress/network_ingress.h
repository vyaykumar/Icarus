#ifndef ICARUS_NETWORK_INGRESS_H
#define ICARUS_NETWORK_INGRESS_H

#include "ingress_interface.h"

class NetworkIngress {
private:
    IngressSource* source;

public:
    NetworkIngress (IngressSource* source);

    const uint8_t* process_packet() {
        if (!source->has_data()) return nullptr;
        return source->get_next_packet();
    }

    int32_t packet_size () const {
        return source->packet_size();
    }

    void shutdown();
};

#endif //ICARUS_NETWORK_INGRESS_H
