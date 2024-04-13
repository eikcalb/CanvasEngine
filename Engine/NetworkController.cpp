#include "NetworkController.h"

const std::string NetworkController::EVENT_TYPE_NEW_MESSAGE = "network_controller_new_message";

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
					// This is an atomic operation (or probably not, looks unnecessary):
					// https://studiofreya.com/cpp/volatile-and-atomic-variables-in-cpp/.
					// TODO: Is this really required???
					// Since only inserts are occuring, will the memory not be expanded
					// when accessed at the same time?
					// Since the memory is a single entity, if multiple threads access it,
					// will it not control and handle all requests accordingly?
					const auto& input = peer.second->Receive();
					auto queue = messageQueue.load();
					NetworkMessageInfo nmi = { peer.second->GetID(), input };
					auto newMessage = std::make_shared<NetworkMessage>(nmi, EVENT_TYPE_NEW_MESSAGE);
					queue.push(newMessage);
					messageQueue.store(queue);
					},
					TaskType::NETWORK, "NC.HandleIncomingMessages receive message"
				);
			}
		}

	} while (isAlive);
}

void NetworkController::ProcessMessages() {
	// This runs asynchronously, so we will check for new messages in each
	// queue and process those messages.
	do {
		if (sendQueue.load().size() > 0) {
			mThreadController->AddTask([&] {
				auto queue = sendQueue.load();
				OutputDebugString(L"Sending message to peers!");

				while (!queue.empty()) {
					const auto& message = queue.front();
					for (auto& peerEntry : GetPeerMap()) {
						peerEntry.second->Send(message);
					}
					queue.pop();
				}

				OutputDebugString(L"Send succeeded!");

				sendQueue.store(queue);
				},
				TaskType::NETWORK, "NC.ProcessMessages:Handle outgoing messages!"
			);
		}

		if (messageQueue.load().size() > 0) {
			mThreadController->AddTask([&] {
				auto queue = messageQueue.load();

				while (!queue.empty()) {
					auto& message = queue.front();
					BroadcastMessage(dynamic_cast<Message<std::any>*>(message.get()));
					queue.pop();
				}
				},
				TaskType::NETWORK, "NC.ProcessMessages:Handle incoming messages!"
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

void NetworkController::OnMessage(Message<std::any>* msg) {
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

		auto peer = connMsg->getData();
		// We lock here to ensure that while reading the connected peer list,
		// an update does not occur, thereby leading to inconsistent state.
		std::lock_guard lock(peerMx);

		if (PeerCount() > MAX_PEERS) {
			OutputDebugString(L"Connection has reached capacity!");
			peer.reset();
		}

		// Check if the peer already exists.
		if (peers.count(peer->GetID()) > 0) {
			OutputDebugString(L"Connection already exists!");
			peer.reset();
		}

		// Set the socket to non-bloaking.
		u_long mode = 1;
		ioctlsocket(peer->GetSocket(), FIONBIO, &mode);

		// We add the new peer to our list of peers.
		// TODO: use WSAEventSelect to reset a blocking select and listen to this new peer.
		OutputDebugString(L"New connection received!");
		peers[peer->GetID()] = peer;
	}
}

std::shared_ptr<NetworkController> NetworkController::Instance()
{
	static std::shared_ptr<NetworkController> instance{ new NetworkController() };
	return instance;
}
