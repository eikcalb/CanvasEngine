#include "Connection.h"

PeerDetails PeerDetails::fromAddrInfo(addrinfo* adr) {
    std::string ipAddress;
    unsigned short port = 0;

    if (adr->ai_family == AF_INET) {
        sockaddr_in* sockaddrIPv4 = reinterpret_cast<sockaddr_in*>(adr->ai_addr);
        char ipBuffer[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(sockaddrIPv4->sin_addr), ipBuffer, INET_ADDRSTRLEN);
        ipAddress = ipBuffer;
        port = ntohs(sockaddrIPv4->sin_port);
    }
    else if (adr->ai_family == AF_INET6) {
        sockaddr_in6* sockaddrIPv6 = reinterpret_cast<sockaddr_in6*>(adr->ai_addr);
        char ipBuffer[INET6_ADDRSTRLEN];
        inet_ntop(AF_INET6, &(sockaddrIPv6->sin6_addr), ipBuffer, INET6_ADDRSTRLEN);
        ipAddress = ipBuffer;
        port = ntohs(sockaddrIPv6->sin6_port);
    }

    return { port, ipAddress };
}

PeerDetails PeerDetails::fromSocketAddrInet(sockaddr_in* sockAdr) {
    char ipBuffer[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(sockAdr->sin_addr), ipBuffer, INET_ADDRSTRLEN);
    u_short port = ntohs(sockAdr->sin_port);

    return { port, std::string(ipBuffer) };
}

std::string ConnectionStrategy::EVENT_TYPE_STARTED = "network_started";
std::string ConnectionStrategy::EVENT_TYPE_NEW_CONNECTION = "network_new_connection";


void Connection::Send(byte data[]) {

}
