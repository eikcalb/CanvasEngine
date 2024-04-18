#pragma once
#include <functional>
#include <memory>
//#ifndef WIN_SOCK_NON_BLOCK
//	// This fixes the error of importing winsock2 multiple times.
//	#define WIN_SOCK_NON_BLOCK
//	#include <WinSock2.h>
//	#include <WS2tcpip.h>
//	#pragma comment(lib, "Ws2_32.lib")
//#endif // !WIN_SOCK_NON_BLOCK

#include "ObserverSubject.h"

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
	static const std::string EVENT_TYPE_STARTED;
	static const std::string EVENT_TYPE_NEW_CONNECTION;

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
/// 
/// This class allows for fault tolerance by possibly retrying requests that are
/// out of order.
/// </summary>
class Connection : ObserverSubject
{
	// This will indicate the message sequence count and is used to help order
	// the messages received by this connection.
	std::atomic_ulong sequenceCount = 0;
	byte buffer[BUFFER_SIZE];
	/// <summary>
	/// This is the socket associated with a connection.
	/// </summary>
	SOCKET socket;

	std::shared_ptr<PeerDetails> info;

public:
	static const byte SEPARATOR = '\n';

	Connection() = default;

	Connection(std::shared_ptr<PeerDetails> details, SOCKET socket) : info(details), socket(socket) {
		// Create buffer. This will instantiate the buffer.
		memset(buffer, 0, sizeof(buffer));
	}

	~Connection() {
		Disconnect();
	}

	inline const std::string GetID() const { return info->id + ":" + std::to_string(info->port); }
	const SOCKET& GetSocket() const { return socket; }

	inline void Disconnect() {
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
		info.reset();

		OutputDebugString(L"Destroyed connection: ID_");
	}

	/// <summary>
	/// Receive an incoming message from the associated socket. This works by calling `recv`
	/// in a loop until there is no data left. We will make best effort to receive complete
	/// input over the wire, but immediately the end of a message is received, we return
	/// that message. This leaves a possibility for partial messages to be retained in the
	/// buffer when the last message was complete, but contains some bytes for the next
	/// message.In this scenario, it is up to users to parse these network messages and identify
	/// the separator.
	/// 
	/// Implementers may extend this class and provide custom logic for reading
	/// messages over the socket connection (IOCP?). Alternatively, implementers may decide
	/// to only overwrite the `ParseMessage` function and handle the buffer content. In this
	/// scenario, the implementer should return `true` when done and is a signal to allow the
	/// application exit the `Receive` loop.
	/// </summary>
	virtual const std::vector<byte>& Receive();
	/// <summary>
	/// Sends a message to the connection. This is a fire and forget implementation.
	/// Errors will be communicated, but this connection will not handle the errors
	/// and will not attempt to retry sending the data provided. TCP is a good protocol
	/// as it has a guarantee of delivery, unlike UDP. For UDP, the caller is responsible
	/// for handling errors and retrying a send if necessary.
	/// 
	/// This concept was chosen in order to create an efficient near-realtime system.
	/// </summary>
	void Send(const std::vector<byte>& data);

protected:
	
	/// <summary>
	/// Parses the connection buffer and returns `true` to make the assumption that the
	/// complete message has been parsed, preventing the function from being called again
	/// if there is no guarantee that new data will be fetched for the new call.
	/// 
	/// By default, this will parse messages in the buffer by reading the buffer until there
	/// is nothing left to be read without blocking the socket. The application aims to not
	/// retrieve incomplete messages, hence it will continue reading messages until there
	/// is nothing left to be read.
	/// </summary>
	virtual const bool ParseMessage(const int receiveCount, std::vector<byte>& bytes) {
		// Loop through buffer and populate the `bytes` result.
		for (auto i = 0; i < receiveCount; i++) {
			bytes.emplace_back(buffer[i]);
		}

		// Here we are not doing anything complicated. Instead, we are just converting
		// the byte sequence to a vector. this vector will be the messaqge received and
		// the application should not expect more messages.
		//
		// This is a simple implementation. If the intention is to confirm messages are
		// complete, then the buffer could be checked for a terminator and ask for a fresh
		// buffer.
		return true;
	}
};
