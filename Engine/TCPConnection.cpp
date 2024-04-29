#include "TCPConnection.h"

std::shared_ptr<Connection> TCPConnection::ConnectToPeer(const Peer& peer) noexcept {
	std::shared_ptr<Connection> conn;
	struct addrinfo* result = NULL,
		* ptr = NULL,
		hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	if (
		getaddrinfo(
			peer.ipAddress.c_str(),
			std::to_string(peer.port).c_str(),
			&hints,
			&result
		) != NO_ERROR
		) {
		OutputDebugString(L"Failed to get address info ");
		OutputDebugString(std::to_wstring(WSAGetLastError()).c_str());
		// This probably indicates a problem with the networking infrastructure.
		// The application should not continue as it is unlikely that it will
		// ever succeed.
		throw std::runtime_error("Failed to get address info");
	}

	try {
		// First address:
		// https://learn.microsoft.com/en-us/windows/win32/winsock/creating-a-socket-for-the-client
		ptr = result;

		/*sockaddr_in address{};
		address.sin_family = AF_INET;
		address.sin_port = htons(peer.second.port);
		if (inet_pton(AF_INET, peer.second.ipAddress.c_str(), &address.sin_addr) <= 0) {
			OutputDebugString(L"Invalid IP address specified ");
			OutputDebugString(std::to_wstring(WSAGetLastError()).c_str());
			return;
		}*/

		// Given the address, we will now check if the connection is allowed.
		std::shared_ptr<PeerDetails> info = std::make_shared<PeerDetails>(PeerDetails::fromAddrInfo(ptr));
		if (!this->validator(info.get())) {
			// If the connection is not allowed, we will skip and move on to
			// the next peer. This gives implementers the choice of throwing errors
			// inside the validator, thereby exiting the connection attempt if one
			// peer cannot be connected.
			// Although technically this is allowed by sockets, it's similar to
			// having separate tabs open to the same website. We do not want this
			// scenario as I can not begin to think of the inconsistencies that
			// will arise from doing this.
			OutputDebugString(L"Peer connection is not allowed!\r\n");
			freeaddrinfo(result);

			return NULL;
		}

		SOCKET clientSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (clientSocket == INVALID_SOCKET)
		{
			OutputDebugString(L"Failed to create client socket ");
			OutputDebugString(std::to_wstring(WSAGetLastError()).c_str());
			throw std::exception("Failed to create client socket");
		}

		if (connect(clientSocket, ptr->ai_addr, ptr->ai_addrlen) == SOCKET_ERROR) {
			OutputDebugString(L"Failed to connect client socket ");
			OutputDebugString(std::to_wstring(WSAGetLastError()).c_str());
			closesocket(clientSocket);
			clientSocket = INVALID_SOCKET;
			throw std::exception("Failed to connect client socket");
		}

		conn = std::make_shared<Connection>(info, clientSocket);
	}
	catch (std::exception& ex) {
		freeaddrinfo(result);

		// These errors are irrecoverable, but thwy do not depict an error
		// state of the application. Rather ther occur when a connection
		// attempt fails and we can just skip that connection.
		return NULL;
	}

	auto msgInfo = std::make_shared<ConnectionMessageInfo>();
	msgInfo->conn = conn;

	ConnectionMessage* msg = new ConnectionMessage(
		msgInfo,
		EVENT_TYPE_NEW_CONNECTION
	);
	BroadcastMessage(msg);

	freeaddrinfo(result);

	return conn;
}
