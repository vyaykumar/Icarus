#include "ingress.h"

NetworkIngress::NetworkIngress() : socket_fd(-1) {
}

NetworkIngress::~NetworkIngress() {
}

bool NetworkIngress::initialize() {
    return false;
}

void NetworkIngress::receive_packets() {
}

void NetworkIngress::shutdown() {
}