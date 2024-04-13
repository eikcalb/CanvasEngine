#include "Connection.h"

#pragma region Static constant declarations

const std::string ConnectionStrategy::EVENT_TYPE_STARTED = "network_started";
const std::string ConnectionStrategy::EVENT_TYPE_NEW_CONNECTION = "network_new_connection";

#pragma endregion


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

void Connection::Send(byte* data) {

		for (SOCKET peer : mPeers) {
			OutputDebugString(L"Sending message!");
			if (send(peer, message.c_str(), message.length(), 0) == SOCKET_ERROR) {
				OutputDebugString(L"Send failed with ");
				OutputDebugString(std::to_wstring(WSAGetLastError()).c_str());
			}
			else {
				OutputDebugString(L"Send succeeded!");
			}
		}
	}
}

const std::vector<byte>& Connection::Receive() {
	bool done = false;

	std::vector<byte> result{};
	do {
		const auto receiveCount = recv(socket, reinterpret_cast<char*>(buffer), sizeof(buffer), 0);
		if (receiveCount == SOCKET_ERROR)
		{
			auto errorCode = WSAGetLastError();
			if (errorCode != WSAEWOULDBLOCK)
			{
				// Error or connection closed ungracefully
				OutputDebugString(L"Receive failed with ");
				OutputDebugString(std::to_wstring(WSAGetLastError()).c_str());
				throw std::exception("Connection error with peer!");
			}
		}
		else if (receiveCount == 0)
		{
			// Connection closed by the peer
			OutputDebugString(L"Connection gracefully closed with peer.");
			done = true;
			break;
		}

		sequenceCount++;
		done = !ParseMessage(receiveCount, result);
	} while (!done);

	ZeroMemory(buffer, BUFFER_SIZE);

	return result;
}
