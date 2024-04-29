#pragma once

#include "Connection.h"
#include "ConnectionMessage.h"
#include "Message.h"
#include "ResourceController.h"

/// <summary>
/// Initiates a connection to peers using TCP. As this is a connection
/// oriented protocol, peer discovery has to come from a predefined list.
/// </summary>
class TCPConnection : public ConnectionStrategy
{
protected:
	SOCKET		listenSocket;

	std::shared_ptr<Config> config;

public:
	TCPConnection(std::shared_ptr<Config> config) : ConnectionStrategy() {
		this->config = config;

		// This connection strategy will get the list of peers and attempt
		// connecting to these clients. Here, we will attempt to connect
		// while also leaving the socket open to accept connections.
		sockaddr_in listenAddress;
		listenAddress.sin_addr.s_addr = htonl(INADDR_ANY);
		listenAddress.sin_family = AF_INET;
		listenAddress.sin_port = htons(this->config->port);

		listenSocket = socket(listenAddress.sin_family, SOCK_STREAM, IPPROTO_TCP);
		if (listenSocket == INVALID_SOCKET)
		{
			OutputDebugString(L"Failed to create listening socket ");
			OutputDebugString(std::to_wstring(WSAGetLastError()).c_str());
			throw std::exception("Failed to create listening socket");
		}
		else if (bind(listenSocket, reinterpret_cast<sockaddr*>(&listenAddress), sizeof(listenAddress)) == SOCKET_ERROR)
		{
			OutputDebugString(L"Bind failed with ");
			OutputDebugString(std::to_wstring(WSAGetLastError()).c_str());
			closesocket(listenSocket);
			listenSocket = INVALID_SOCKET;
			throw std::exception("Failed to bind socket");
		}
		else if (listen(listenSocket, std::min<short>(SOMAXCONN, MAX_PEERS * 2)) == SOCKET_ERROR)
		{
			OutputDebugString(L"Listen failed with ");
			OutputDebugString(std::to_wstring(WSAGetLastError()).c_str());
			closesocket(listenSocket);
			listenSocket = INVALID_SOCKET;
			throw std::exception("Socket failed to listen");
		}

		started = true;
		Message msg{ EVENT_TYPE_STARTED };
		this->BroadcastMessage(&msg);
	}

	~TCPConnection() {
		started = false;
		auto shutdownResult = shutdown(listenSocket, SD_BOTH);
		if (shutdownResult == SOCKET_ERROR) {
			OutputDebugString(L"Failed to shutdown listener socket.\r\n");
		}

		closesocket(listenSocket);
		listenSocket = INVALID_SOCKET;

		OutputDebugString(L"Destroyed connection: TCPConnection!\r\n");
	}

	std::shared_ptr<Connection> ConnectToPeer(const Peer& peer) noexcept;

	virtual std::vector<std::shared_ptr<Connection>> Connect() {
		if (!validator) {
			throw std::exception("Connection validator has not been specified!");
		}

		// Here we will need to connect to the peers listed out in the config.
		//std::shared_ptr<Connection> peersConn[MAX_PEERS];
		std::vector<std::shared_ptr<Connection>> peersConn;
		peersConn.reserve(MAX_PEERS);

		if (config->peers.size() > MAX_PEERS) {
			throw std::exception(("Peers registered must be less than " + std::to_string(MAX_PEERS)).c_str());
		}

		for (auto const& peer : config->peers) {
			// Connect to peer.
			auto newConn = ConnectToPeer(peer.second);
			if (newConn) {
				peersConn.emplace_back(newConn);
			}
		}

		return peersConn;
	}

	virtual void Start() {
		if (!validator) {
			throw std::runtime_error("Connection validator has not been specified!");
		}
		// For TCP, we will listen for connections to this socket and
		// create these connections. For a connection strategy, we do
		// not care about sending and receiving data from client sockets.
		// instead that will be handled by the connection itself. Here,
		// we will only start a "promiscuous" search for peers trying to
		// connect with this application.
		fd_set readSockets = {};

		do {
			FD_ZERO(&readSockets);
			FD_SET(listenSocket, &readSockets);

			int socketCount = select(-1/* unused on Windows */, &readSockets, nullptr, nullptr, nullptr);
			if (socketCount == SOCKET_ERROR)
			{
				OutputDebugString(L"Failed to run select()!\r\n");
				OutputDebugString(std::to_wstring(WSAGetLastError()).c_str());
				// If the listening socket failed to read, it is probably
				// a problem of the underlying socket and it should be
				// handled by the application.
				// Further processing of the loop or future loops would make
				// no sense.
				throw std::runtime_error("Failed to accept connection!");
			}

			if (!FD_ISSET(listenSocket, &readSockets)) {
				// If the only socket in the list is not set and the select
				// returned, then I am not certain what was read. At this point,
				// exit the loop and give the application a chance to recover.
				OutputDebugString(L"Listening socket is in an unrecognized state!\r\n");
				throw std::runtime_error("Unrecognized state encountered!");
			}

			sockaddr_in clientAddr{};
			int addrSize = sizeof(clientAddr);
			SOCKET clientSocket = accept(listenSocket, reinterpret_cast<sockaddr*>(&clientAddr), &addrSize);
			if (clientSocket == INVALID_SOCKET) {
				OutputDebugString(L"Invalid socket accepted!\r\n");
				continue;
			}

			std::shared_ptr<PeerDetails> info = std::make_shared<PeerDetails>(PeerDetails::fromSocketAddrInet(&clientAddr));
			if (!validator(info.get())) {
				OutputDebugString(L"Validation failed for connection!\r\n");
				closesocket(clientSocket);
				clientSocket = INVALID_SOCKET;
				continue;
			}

			auto conn = std::make_shared<Connection>(info, clientSocket);

			auto msgInfo = std::make_shared<ConnectionMessageInfo>();
			msgInfo->conn = conn;

			ConnectionMessage* msg = new ConnectionMessage(
				msgInfo,
				EVENT_TYPE_NEW_CONNECTION
			);
			// Broadcast new connection to listeners.
			BroadcastMessage(msg);
		} while (started);
	}
};

