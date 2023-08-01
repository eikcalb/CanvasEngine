#include "NetworkController.h"

NetworkController::NetworkController()
	:
	peerBuffers(),
	messages(),
	backlog()
{
	mPeers.reserve(5);
}

void NetworkController::HandleIncomingMessage(const SOCKET& peerSocket)
{
	PeerBuffer& peerBuffer = peerBuffers[peerSocket];
	int			bytesReceived = 0;

	// Receive the incoming message
	char		buffer[BUFFER_SIZE];
	bytesReceived = recv(peerSocket, buffer, sizeof(buffer), 0);
	if (bytesReceived == SOCKET_ERROR)
	{
		int errorCode = WSAGetLastError();
		if (errorCode != WSAEWOULDBLOCK)
		{
			// Error or connection closed by the peer
			OutputDebugString(L"Receive failed with ");
			OutputDebugString(std::to_wstring(WSAGetLastError()).c_str());
			closesocket(peerSocket);
			peerBuffers.erase(peerSocket);
			return;
		}
	}
	else if (bytesReceived == 0)
	{
		// Error or connection closed by the peer
		OutputDebugString(L"Receive failed with ");
		OutputDebugString(std::to_wstring(WSAGetLastError()).c_str());
		closesocket(peerSocket);
		peerBuffers.erase(peerSocket);
		return;
	}

	// Append the received data to the buffer
	memcpy(peerBuffer.buffer + peerBuffer.length, buffer, bytesReceived);
	peerBuffer.length += bytesReceived;

	// This will read messages in the socket until a terminator is encountered.
	// That will identify the end of a message stream and allow us start accepting
	// a "new" line of messages.
	size_t pos = 0;
	while (pos < peerBuffer.length) {
		// Check if we have a complete message in the buffer
		size_t messageEnd = std::string::npos;
		for (size_t i = pos; i < bytesReceived; ++i)
		{
			if (buffer[i] == '\n')
			{
				messageEnd = i;
				break;
			}
		}

		if (messageEnd != std::string::npos) {
			// Extract the complete message
			// This gets the message between the start of the buffer and until the end.
			// `pos` here would be `0` for the first message in the buffer, if there are
			// more messages in the buffer then `pos` will be set to the byte just after
			// the previous message.
			std::string message(peerBuffer.buffer + pos, messageEnd - pos);
			{
				std::lock_guard<std::mutex> lock(mx);
				messages.push(message);
			}

			// Update the position for the next message
			pos = messageEnd + 1;
		}
		else
		{
			// This runs when the end of a message was not found. This could indicate a longer
			// message or the second part of a previously parsed message. In this scenario,
			// we want to parse the message and continue the loop until we get to it's end.
			size_t remainingBytes = peerBuffer.length - pos;
			memmove(peerBuffer.buffer, peerBuffer.buffer + pos, remainingBytes);
			peerBuffer.length = remainingBytes;
			break;
		}
	}
}

void NetworkController::SendMessages(const std::queue<std::string> queue)
{
	std::queue<std::string> tempQueue = queue;
	while (!tempQueue.empty())
	{
		const std::string& message = queue.front();
		tempQueue.pop();

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

void NetworkController::FindPeers()
{
	std::shared_ptr<Config>		config = mResourceController->getConfig();
	std::map<std::string, Peer>	peers;

	sockaddr_in address{};
	address.sin_family = AF_INET;

	for (auto const& peer : peers)
	{
		address.sin_port = htons(peer.second.port);
		inet_pton(address.sin_family, peer.second.ipAddress.c_str(), &address.sin_addr);
		if (inet_pton(AF_INET, peer.second.ipAddress.c_str(), &address.sin_addr) <= 0) {
			OutputDebugString(L"Invalid IP address.");
			return;
		}

		SOCKET peerSocket = socket(AF_INET, SOCK_STREAM, 0);
		if (peerSocket == INVALID_SOCKET)
		{
			OutputDebugString(L"Failed to create socket ");
			OutputDebugString(std::to_wstring(WSAGetLastError()).c_str());
		}
		else if (connect(peerSocket, (sockaddr*)&address, sizeof(address)) == SOCKET_ERROR)
		{
			OutputDebugString(L"Connecting to peer failed with ");
			OutputDebugString(std::to_wstring(WSAGetLastError()).c_str());
			closesocket(peerSocket);
		}
		else
		{
			{
				std::lock_guard<std::mutex> lock(peerMx);
				mPeers.push_back(peerSocket);
				OutputDebugString(L"Connected to peer!");
			}
		}
	}
}

NetworkController::~NetworkController()
{
	for (SOCKET peerSocket : mPeers) {
		closesocket(peerSocket);
	}
	OutputDebugString(L"Peers disconnected!");
	peerBuffers.clear();

	WSACleanup();
}

void NetworkController::InitWinSock()
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)
	{
		OutputDebugString(L"Failed to initialize Winsock");
	}

	FindPeers();

	mListenAddress.sin_family = AF_INET;
	mListenAddress.sin_port = htons(mResourceController->getConfig()->port);
	mListenAddress.sin_addr.s_addr = INADDR_ANY;

	//Create listening socket
	mListenSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (mListenSocket == INVALID_SOCKET)
	{
		OutputDebugString(L"Failed to create listening socket ");
		OutputDebugString(std::to_wstring(WSAGetLastError()).c_str());
		WSACleanup();

		throw std::exception("Failed to create listening socket");
	}
	else if (bind(mListenSocket, reinterpret_cast<sockaddr*>(&mListenAddress), sizeof(mListenAddress)) == SOCKET_ERROR)
	{
		OutputDebugString(L"Bind failed with ");
		OutputDebugString(std::to_wstring(WSAGetLastError()).c_str());
		closesocket(mListenSocket);
		WSACleanup();
		throw std::exception("Failed to bind socket");
	}
	else if (listen(mListenSocket, std::min<short>(SOMAXCONN, MAX_PEERS)) == SOCKET_ERROR)
	{
		OutputDebugString(L"Listen failed with ");
		OutputDebugString(std::to_wstring(WSAGetLastError()).c_str());
		throw std::exception("Socket failed to listen");
	}
	else
	{
		mThreadController->AddTask([this]() {this->ListenToPeer(); }, TaskType::NETWORK);
		mThreadController->AddTask([this]() {this->ProcessBacklog(); }, TaskType::NETWORK);
	}
}

void NetworkController::AddMessage(const std::string& pMessage)
{
	std::lock_guard<std::mutex> lock(addMx);
	backlog.push(pMessage + '\n');
}

std::queue<std::string> NetworkController::ReadMessages()
{
	//Swaps the queue with an empty queue to empty the current queue and returns the queue containing messages
	std::queue<std::string> messages;
	std::lock_guard<std::mutex> lock(mx);
	messages.swap(messages);
	return messages;
}

int NetworkController::PeerCount()
{
	return mPeers.size();
}

std::shared_ptr<NetworkController> NetworkController::Instance()
{
	static std::shared_ptr<NetworkController> instance{ new NetworkController() };
	return instance;
}
