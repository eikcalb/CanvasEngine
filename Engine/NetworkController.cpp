#include "NetworkController.h"

const std::string NetworkController::EVENT_TYPE_NEW_MESSAGE = "network_controller_new_message";

NetworkController::NetworkController() : backlog(),
messages(),
messageQueue(),
peers() {
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

void NetworkController::HandleIncomingMessages()
{
	// Here we will select the peer sockets and read their data individually.
	fd_set readSockets = {};

	do {
		if (PeerCount() < 0) {
			OutputDebugString(L"No peer connected yet!");
			continue;
		}

		FD_ZERO(&readSockets);

		for (const auto& peer : peers) {
			// Populate the file descriptor list.
			FD_SET(peer.second->GetSocket(), &readSockets);
		}

		int socketCount = select(-1, &readSockets, nullptr, nullptr, nullptr);
		if (socketCount == SOCKET_ERROR)
		{
			OutputDebugString(L"Failed to run select()");
			OutputDebugString(std::to_wstring(WSAGetLastError()).c_str());
			// If the listening socket failed to read, it is probably
			// a problem of the underlying socket and it should be
			// handled by the application.
			// Further processing of the loop or future loops would make
			// no sense.
			throw std::runtime_error("Failed to read incoming message!");
		}

		for (auto peer : GetPeerMap()) {
			if (FD_ISSET(peer.second->GetSocket(), &readSockets)) {
				mThreadController->AddTask([&] {
					// Here, we will fetch th enew message and add it to
					// the message queue.
					const auto& input = peer.second->Receive();
					if (input.size() <= 0) {
						OutputDebugString(L"Received an empty message");
						return;
					}

					std::lock_guard lock(messageMx);
					std::shared_ptr<NetworkMessageInfo> nmi = std::make_shared<NetworkMessageInfo>();
					nmi->message = input;
					nmi->peerID = peer.second->GetID();

					auto newMessage = std::make_shared<NetworkMessage>(nmi, EVENT_TYPE_NEW_MESSAGE);
					messageQueue.push(newMessage);
					},
					TaskType::NETWORK,
					"NC.HandleIncomingMessages receive message"
				);
			}
		}

	} while (isAlive);
}

void NetworkController::ProcessMessages() {
	// This runs asynchronously, so we will check for new messages in each
	// queue and process those messages.
	do {
		if (sendQueue.size() > 0) {
			mThreadController->AddTask([&] {
				std::lock_guard lock(sendMx);

				OutputDebugString(L"Sending message to peers!");

				while (!sendQueue.empty()) {
					const auto& message = sendQueue.front();
					for (auto& peerEntry : GetPeerMap()) {
						peerEntry.second->Send(message);
					}
					sendQueue.pop();
				}

				OutputDebugString(L"Send succeeded!");
				},
				TaskType::NETWORK,
				"NC.ProcessMessages:Handle outgoing messages!"
			);
		}

		if (messageQueue.size() > 0) {
			mThreadController->AddTask([&] {
				std::lock_guard lock(messageMx);

				OutputDebugString(L"Reading messages from peers!");

				while (!messageQueue.empty()) {
					auto& message = messageQueue.front();
					BroadcastMessage(message.get());
					messageQueue.pop();
				}
				},
				TaskType::NETWORK,
				"NC.ProcessMessages:Handle incoming messages!"
			);
		}
	} while (isAlive);
}

void NetworkController::SendMessage(const std::vector<byte> message)
{
	OutputDebugString(L"Sending message to peers!");
	for (auto& peerEntry : GetPeerMap()) {
		peerEntry.second->Send(message);
	}
	OutputDebugString(L"Send succeeded!");
}

void NetworkController::Start()
{
	// Start the  network controller by connecting to peers and listening
	// for messages.
	// @PersistentThreadCost = 1
	mThreadController->AddTask([&] {
		connectionStrategy->Connect();
		},
		TaskType::NETWORK, "NC.Start:Connect to peers!"
	);


	// While we are connecting to peers, we will listen to the sockets and
	// trigger the peer connections to read data.
	// @PersistentThreadCost = 4
	if (communicationConfig->promiscuous) {
		OutputDebugString(L"Running in promiscuous mode!");

		mThreadController->AddTask([&] {
			connectionStrategy->Start();
			},
			TaskType::NETWORK, "NC.Start:Listen for new connections!"
		);
	}

	mThreadController->AddTask([&] {
		HandleIncomingMessages();
		},
		TaskType::NETWORK, "NC.Start:Handle incoming messages!"
	);

	mThreadController->AddTask([&] {
		ProcessMessages();
		},
		TaskType::NETWORK, "NC.Start:Process messages!"
	);
}

//std::queue<std::string> NetworkController::ReadMessages()
//{
//	//Swaps the queue with an empty queue to empty the current queue and returns the queue containing messages
//	std::queue<std::string> messages;
//	std::lock_guard<std::mutex> lock(mx);
//	this->messages.swap(messages);
//	return messages;
//}

int NetworkController::PeerCount()
{
	return peers.size();
}

void NetworkController::OnMessage(Message* msg) {
	// When a message is received, we want to handle it here.
	const auto& msgType = msg->GetMessageType();
	if (msgType == ConnectionStrategy::EVENT_TYPE_NEW_CONNECTION) {
		// A new connection has been made. Here we will add the connection
		// to our peer list.
		const auto& connMsg = reinterpret_cast<ConnectionMessage*>(msg);
		if (!connMsg) {
			OutputDebugString(L"Invalid peer connection received!");
			return;
		}

		ConnectionMessageInfo* peer = reinterpret_cast<ConnectionMessageInfo*>(connMsg->getData().get());
		// We lock here to ensure that while reading the connected peer list,
		// an update does not occur, thereby leading to inconsistent state.
		std::lock_guard lock(peerMx);

		if (PeerCount() > MAX_PEERS) {
			OutputDebugString(L"Connection has reached capacity!");
			peer->conn.reset();
		}

		// Check if the peer already exists.
		if (peers.count(peer->conn->GetID()) > 0) {
			OutputDebugString(L"Connection already exists!");
			peer->conn.reset();
		}

		// Set the socket to non-bloaking.
		u_long mode = 1;
		ioctlsocket(peer->conn->GetSocket(), FIONBIO, &mode);

		// We add the new peer to our list of peers.
		// TODO: use WSAEventSelect to reset a blocking select and listen to this new peer.
		OutputDebugString(L"New connection received!");
		peers[peer->conn->GetID()] = peer->conn;
	}
}

std::shared_ptr<NetworkController> NetworkController::Instance()
{
	static std::shared_ptr<NetworkController> instance{ new NetworkController() };
	return instance;
}
