#ifndef ICARUS_INGRESS_HPP
#define ICARUS_INGRESS_HPP

#include <cstdint>

class IIngressReceiver {
public:
    virtual ~IIngressReceiver() = default;

    virtual std::size_t poll(uint8_t* destination_buffer, std::size_t max_len) = 0;
};

#endif // ICARUS_INGRESS_HPP