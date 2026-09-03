#ifndef ICARUS_INGRESS_H
#define ICARUS_INGRESS_H

#include <cstdint>
#include <sys/socket.h>

class NetworkIngress {
private:
    int socket_fd;

public:
    NetworkIngress();
    ~NetworkIngress();

    bool initialize();
    void receive_packets();
    void shutdown();
};

#endif // ICARUS_INGRESS_H