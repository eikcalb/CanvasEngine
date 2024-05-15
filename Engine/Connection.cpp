#include "Connection.h"

#include "NetworkMessage.h"

#pragma region Static constant declarations

const std::string ConnectionStrategy::EVENT_TYPE_STARTED = "network_started";
const std::string ConnectionStrategy::EVENT_TYPE_NEW_CONNECTION = "network_new_connection";

const std::string Connection::EVENT_TYPE_CLOSED_CONNECTION = "network_closed_connection";

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

void Connection::Send(const std::vector<byte>& data) {
	const char* buffer = reinterpret_cast<const char*>(data.data());
	int length = static_cast<int>(data.size());

	// Send data over the socket
	if (send(socket, buffer, length, 0) == SOCKET_ERROR) {
		// Handle send error
		auto errorCode = WSAGetLastError();
		OutputDebugString(L"Send failed with ");
		OutputDebugString(std::to_wstring(errorCode).c_str());
		if (errorCode == WSAECONNRESET || errorCode == WSAENOTCONN) {
			Disconnect();
		}
	}
}

void Connection::Disconnect() {
	if (socket == INVALID_SOCKET) {
		// Probably already disconnected.
		return;
	}

	auto shutdownResult = shutdown(socket, SD_BOTH);
	if (shutdownResult == SOCKET_ERROR) {
		OutputDebugString(L"Failed to shutdown socket.");
	}

	closesocket(socket);
	socket = INVALID_SOCKET;

	OutputDebugString(L"Destroyed connection: ID_");
	auto msgInfo = std::make_shared<NetworkMessageInfo>();
	msgInfo->peerID = info->GetIDString();

	auto msg = new NetworkMessage(
		msgInfo,
		EVENT_TYPE_CLOSED_CONNECTION
	);
	BroadcastMessage(msg);
	ClearAllListeners();
	info.reset();
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
				// Error or connection closed ungracefully.
				OutputDebugString(L"Receive failed with ");
				OutputDebugString(std::to_wstring(WSAGetLastError()).c_str());
				Disconnect();
				return result;
				throw std::exception("Connection error with peer!");
			}
			else {
				// Nothing left to read, but socket may still be active.
			}
		}
		else if (receiveCount == 0)
		{
			// Connection closed by the peer.
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
