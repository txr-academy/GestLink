#include "lwip/sockets.h"

// This is the "glue" that lets Paho talk to the Ethernet hardware
int transport_sendPacketBuffer(int sock, unsigned char* buf, int buflen) {
    // Standard LwIP socket send command
    return send(sock, buf, buflen, 0);
}

int transport_getdata(int sock, unsigned char* buf, int count) {
    // Standard LwIP socket receive command
    return recv(sock, buf, count, 0);
}
