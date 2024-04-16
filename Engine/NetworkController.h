#pragma once
#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <memory>
#include <iostream>
#include <queue>
#include <string>
#include <fstream>
#include <sstream>
#include <unordered_map>

#include "Connection.h"
#include "ConnectionMessage.h"
#include "InputController.h"
#include "NetworkMessage.h"
#include "Observer.h"
#include "ResourceController.h"
#include "ThreadController.h"

typedef std::queue<std::vector<byte>> OutgoingMessagesQueue;
typedef std::queue <std::shared_ptr< NetworkMessage >> IncomingMessagesQueue;
typedef std::unordered_map<std::string, std::shared_ptr<Connection>> PeerMap;

/// <summary>
/// Network controller contains logic for connectivity. The concept is to have a controller object
/// that is configurable. Firstly, the connection establishment mechanism is modular and allow each
/// implementation to specify how connections are done. Second concept is to have a configuration
/// which is used to specify how the network should be controlled. The final concept implemented
/// is that there is no immediate listener. Instead, messages are stored in a queue and a separate
/// thread reads from this queue in a loop, triggerring listeners. Thereby creating a pull-push
/// mechanism.
/// 
/// The order of events will follow:
/// - Set connection mechanism.
/// - Connect to peers based on the configuration specified.
/// - When messages are received, add them to the `messageQueue`.
/// - In a separate thread(s?), these messages will be dispatched (pushed) to consumers.
/// </summary>
class NetworkController : ObserverSubject, public Observer
{
private:
	std::shared_ptr<InputController> mInputController;
	std::shared_ptr<ThreadController> mThreadController;
	std::shared_ptr<ResourceController> mResourceController;

	PeerMap									peers;
	std::queue<std::string>					messages;
	std::queue<std::string>					backlog;


	std::shared_ptr<CommunicationConfig> communicationConfig;
	std::shared_ptr<ConnectionStrategy> connectionStrategy;

	//std::mutex messageMx;
	std::mutex peerMx;

	std::atomic<IncomingMessagesQueue> messageQueue;
	std::atomic<OutgoingMessagesQueue> sendQueue;
	std::atomic<bool> isAlive = false;

	//Private constructor for singleton pattern
	NetworkController()
		:
		backlog(),
		messages(),
		messageQueue(),
		peers()
	{
		mInputController = InputController::Instance();
		mThreadController = ThreadController::Instance();
		mResourceController = ResourceController::Instance();

		isAlive = true;

		peers.reserve(MAX_PEERS);

		// Initialize winsock dll
		WSADATA wsaData;
		WORD winsockVersion = MAKEWORD(2, 2);
		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != NO_ERROR)
		{
			OutputDebugString(L"Failed to initialize Winsock!");
			OutputDebugString(L"Nothing network-relatred will work for this application!");
		}
	}

	void HandleIncomingMessages();
	/// <summary>
	/// In order to prevent continuously waiting on messages for input and output,
	/// we will utilize a single persistent thread that will only create tasks for
	/// reading messages when there is a message.
	/// </summary>
	void ProcessMessages();
public:
	static std::shared_ptr< NetworkController > Instance();

	~NetworkController()
	{
		isAlive = false;

		OutputDebugString(L"Network Terminated!");

		WSACleanup();
	}

	//Singleton pattern
	NetworkController(const NetworkController& NetworkController) = delete;
	NetworkController& operator=(NetworkController const&) = delete;

	/// This function will end and the following are guaranteed:
	/// - Peers must have been atleast attempted to be connected to.
	/// - If connection strategy is promiscuous, then the socked is
	///   set to start accepting connections.
	/// - The incoming and outgoing message queues are setup to be
	///   checked in a loop and handled accordingly.
	void					Start();

	void					SendMessage(const std::vector<byte> message);
	int						PeerCount();

	virtual void OnMessage(Message<std::any>*);

	const PeerMap& GetPeerMap() const { return peers; }

	void SetCommunicationConfig(CommunicationConfig* commConfig) {
		communicationConfig = std::shared_ptr<CommunicationConfig>(commConfig);
	}

	void SetConnectionStrategy(ConnectionStrategy* connStrategy) {
		connectionStrategy = std::shared_ptr<ConnectionStrategy>(connStrategy);
	}

public:
	static const std::string EVENT_TYPE_NEW_MESSAGE;
};