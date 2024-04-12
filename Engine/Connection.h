#pragma once
#include <functional>
#include <memory>
#include <WinSock2.h>
#include <WS2tcpip.h>

#include "Message.h"
#include "ObserverSubject.h"

#pragma comment(lib, "Ws2_32.lib")

constexpr u_short BUFFER_SIZE = 1024;
constexpr u_short MAX_PEERS = 8;

struct PeerDetails{
	u_short port;
	std::string id;

	static PeerDetails fromAddrInfo(addrinfo* adr);
	static PeerDetails fromSocketAddrInet(sockaddr_in* sockAdr);
};

/// <summary>
/// Validates a connection before creating a socket with each peer.
/// A scenario where this will come in handy is to prevent connection
/// attempts to clients that are already connected. Implementers should
/// return `true` if the connection shoul dcontinue or `false` otherwise.
/// This should not stop the flow of execution, it should instead skip
/// connecting to a particular peer as the peer does not match a criteron.
/// </summary>
typedef std::function<bool(PeerDetails* peerAddressInfo)> ConnectionValidator;

// Forward declaring `Connection` class.
class Connection;

/// <summary>
/// Contains logic for creating connections. Subclasses are responsible
/// for creating connections and returning those connections.
/// 
/// This is an abstract class and subclasses will be responsible for
/// the fine grained details for connectivity. For example, one subclass
/// may use a configuration file to connect with peers, while another might
/// utilize UDP multicast for peer discovery and connectivity.
/// </summary>
class ConnectionStrategy : protected ObserverSubject {
protected:
	std::atomic<bool> started = false;
	ConnectionValidator validator;

public:
	static std::string EVENT_TYPE_STARTED;
	static std::string EVENT_TYPE_NEW_CONNECTION;

public:
	ConnectionStrategy() :started(false) {}

	void SetConnectionValidator(ConnectionValidator validator) {
		this->validator = validator;
	}

	/// <summary>
	/// Called when the network infrastructure is set to not connect
	/// continuously (not in promiscuous mode). In this mode, each peer
	/// will be connected to one at a time.
	/// </summary>
	/// <returns>Connection</returns>
	virtual std::vector<std::shared_ptr<Connection>> Connect() = 0;

	/// <summary>
	/// When the network configuration is set to be in promiscuous mode,
	/// this method is called to start connecting to peers. Upon successful
	/// connection, each subclass is responsible for notifying listeners of
	/// the new connection.
	/// </summary>
	virtual void Start() = 0;
};

struct CommunicationConfig {
	// When true, signifies that the communuication mechanism will
	// be continuous and new connections should be handled.
	// When false, then the network connections should be handled
	// manually (such as on startup).
	// 
	// The concept behind this is that a connection can be made using
	// a predefined list on startup or the application can go into
	// "listen" mode. For TCP, the assumption is that when the application
	// is not in listen mode it might only be able to connect with peers
	// that have been defined by some other means. When listening, the
	// connection will accept peers over the specified port.
	// For UDP, a "listen" state might be to broadcast multicast messages
	// and accept peers that attempt to connect with the server.
	//
	// Peers must be in promiscuous mode else, the connection will never work.
	bool promiscuous;
};

/// <summary>
/// Peer connection implementation. This is responsible for sending, receiving
/// an managing the lifecycle of events used to interact with peers.
/// 
/// Ideally, this class should be able to handlevarious network configurations
/// as it's sole responsibility is to send and receive data. Protocol specific
/// details should be handled in the `ConnectionStrategy`.
/// </summary>
class Connection : ObserverSubject
{
	// This will indicate the message sequence count and is used to help order
	// the messages received by this connection.
	u_long sequenceCount = 0;
	byte buffer[BUFFER_SIZE];
	/// <summary>
	/// This is the socket associated with a connection.
	/// </summary>
	SOCKET socket;
	std::shared_ptr<PeerDetails> info;

public:
	unsigned int bufferLength = 0;

	Connection() = default;

	Connection(std::shared_ptr<PeerDetails> details, SOCKET socket) : info(details), socket(socket) {
		// Create buffer. This will instantiate the buffer.
		memset(buffer, 0, sizeof(buffer));
	}

	~Connection() {
		auto shutdownResult = shutdown(socket, SD_BOTH);
		if (shutdownResult == SOCKET_ERROR) {
			OutputDebugString(L"Failed to shutdown socket.");
		}

		closesocket(socket);
		socket = INVALID_SOCKET;

		OutputDebugString(L"Destroyed connection: ID_");
	}

	inline std::string GetID() { return info->id + ":" + std::to_string(info->port); }

	byte* Receive();
	void Send(byte data[]);
};

