#include "NetworkController.h"

#include "NetworkMessage.h"
#include "KeyPressMessage.h"
#include "Utils.h"

const std::string NetworkController::EVENT_TYPE_NEW_MESSAGE = "network_controller_new_message";

NetworkController::NetworkController() :
	backlog(),
	messages(),
	messageQueue(),
	peers()
{
	mInputController = InputController::Instance();
	mThreadController = ThreadController::Instance();
	mResourceController = ResourceController::Instance();

	isAlive = true;

	peers->reserve(MAX_PEERS);

	// Initialize winsock dll
	WSADATA wsaData;
	WORD winsockVersion = MAKEWORD(2, 2);
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != NO_ERROR)
	{
		OutputDebugString(L"Failed to initialize Winsock!\r\n");
		OutputDebugString(L"Nothing network-relatred will work for this application!\r\n");
	}
}

void NetworkController::HandleIncomingMessages()
{
	// Here we will select the peer sockets and read their data individually.
	fd_set readSockets = {};

	do {
		if (PeerCount() <= 0) {
			// OutputDebugString(L"No peer connected yet!\r\n");
			continue;
		}

		FD_ZERO(&readSockets);

		for (const std::pair< std::string, std::shared_ptr<Connection>> peer : *peers) {
			// Populate the file descriptor list.
			FD_SET(peer.second->GetSocket(), &readSockets);
		}

		int socketCount = select(-1, &readSockets, nullptr, nullptr, nullptr);
		if (socketCount == SOCKET_ERROR)
		{
			int errorCode = WSAGetLastError();
			OutputDebugString(L"Failed to run select(): ");
			OutputDebugString(std::to_wstring(errorCode).c_str());
			OutputDebugString(L"\r\n");

			// Check each socket in the readSockets set
			for (const auto& peer : GetPeerMap()) {
				if (FD_ISSET(peer.second->GetSocket(), &readSockets)) {
					// Check if the socket is still valid
					int error = 0;
					socklen_t len = sizeof(error);
					if (getsockopt(peer.second->GetSocket(), SOL_SOCKET, SO_ERROR, reinterpret_cast<char*>(&error), &len) == 0) {
						// The socket is still valid, but select failed for some other reason
						continue;
					}
					else {
						peer.second->Disconnect();
					}
				}
			}

			// If the listening socket failed to read, it is probably
			// a problem of the underlying socket and it should be
			// handled by the application.
			// Further processing of the loop or future loops would make
			// no sense.
			// However, this tends to indicate that a socket might be dead,
			// but we are yet to receive an event for it.
			continue;
		}

		for (const auto& peer : *peers) {
			Connection* conn = peer.second.get();

			if (FD_ISSET(conn->GetSocket(), &readSockets)) {
				mThreadController->AddTask([&] {
					// Here, we will fetch the new message and add it to
					// the message queue.
					if (!conn) {
						return;
					}

					const auto& input = conn->Receive();
					if (input.size() <= 0) {
						//OutputDebugString(L"Received an empty message\r\n");
						return;
					}

					std::lock_guard lock(messageMx);
					std::shared_ptr<NetworkMessageInfo> nmi = std::make_shared<NetworkMessageInfo>();
					nmi->message = input;
					nmi->peerID = conn->GetID();

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

	// Manage frequency.
	double start = Utils::GetTime();
	do {
		auto now = Utils::GetTime();
		auto dt = now - start;

		if (dt >= 1.0 / _fps) {
			if (sendQueue.size() > 0) {
				mThreadController->AddTask([&] {
					std::lock_guard lock(sendMx);

					OutputDebugString(L"Sending message to peers!\r\n");

					while (!sendQueue.empty()) {
						const auto& message = sendQueue.front();
						for (auto& peerEntry : GetPeerMap()) {
							peerEntry.second->Send(message);
						}
						sendQueue.pop();
					}

					OutputDebugString(L"Send succeeded!\r\n");
					},
					TaskType::NETWORK,
					"NC.ProcessMessages:Handle outgoing messages!"
				);
			}

			if (messageQueue.size() > 0) {
				mThreadController->AddTask([&] {
					std::lock_guard lock(messageMx);

					OutputDebugString(L"Reading messages from peers!\r\n");

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
			_actualfps = 1.0 / (dt);
			start = now;
		}
	} while (isAlive);
}

void NetworkController::SendMessage(const std::vector<byte> message)
{
	OutputDebugString(L"Sending message to peers!\r\n");
	for (auto& peerEntry : GetPeerMap()) {
		peerEntry.second->Send(message);
	}
	OutputDebugString(L"Send succeeded!\r\n");
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
		OutputDebugString(L"Running in promiscuous mode!\r\n");

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

int NetworkController::PeerCount()
{
	return peers->size();
}

void NetworkController::OnMessage(Message* msg) {
	// When a message is received, we want to handle it here.
	const auto& msgType = msg->GetMessageType();
	if (msgType == ConnectionStrategy::EVENT_TYPE_NEW_CONNECTION) {
		// A new connection has been made. Here we will add the connection
		// to our peer list.
		const auto& connMsg = reinterpret_cast<ConnectionMessage*>(msg);
		if (!connMsg) {
			OutputDebugString(L"Invalid peer connection received!\r\n");
			return;
		}

		{
			std::lock_guard lock(peerMx);
			ConnectionMessageInfo* peer = reinterpret_cast<ConnectionMessageInfo*>(connMsg->getData().get());
			// We lock here to ensure that while reading the connected peer list,
			// an update does not occur, thereby leading to inconsistent state.

			if (PeerCount() > MAX_PEERS) {
				OutputDebugString(L"Connection has reached capacity!\r\n");
				peer->conn.reset();
				return;
			}

			// Check if the peer already exists.
			if (peers->count(peer->conn->GetID()) > 0) {
				OutputDebugString(L"Connection already exists!\r\n");
				peer->conn.reset();
				return;
			}

			// Set the socket to non-bloaking.
			u_long mode = 1;
			ioctlsocket(peer->conn->GetSocket(), FIONBIO, &mode);

			// We add the new peer to our list of peers.
			// TODO: use WSAEventSelect to reset a blocking select and listen to this new peer.
			OutputDebugString(L"New connection received!\r\n");
			peers->insert({ peer->conn->GetID(), peer->conn });
			peer->conn->Observe(Connection::EVENT_TYPE_CLOSED_CONNECTION, std::shared_ptr<NetworkController>(this));
		}
	}
	else if (msgType == Connection::EVENT_TYPE_CLOSED_CONNECTION) {
		std::lock_guard lock(peerMx);
		auto networkMessage = reinterpret_cast<NetworkMessage*>(msg);
		const std::string id = networkMessage->GetMessage()->peerID;
		peers->erase(id);
	}
	else if (msgType == InputController::EVENT_KEY_INPUT) {
		const auto& keyMsg = reinterpret_cast<KeyPressMessage*>(msg);
		KEYS key = static_cast<KEYS>(keyMsg->GetKey());
		if (!keyMsg->GetDown()) {
			if (key == KEYS::C) {
				mThreadController->AddTask([&] {
					connectionStrategy->Connect();
					},
					TaskType::NETWORK, "NC.OnMessage:Connect to peers again!"
				);
			}
		}
	}
}

std::shared_ptr<NetworkController> NetworkController::Instance()
{
	static std::shared_ptr<NetworkController> instance{ new NetworkController() };
	return instance;
}
