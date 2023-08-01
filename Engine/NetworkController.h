#pragma once
#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <memory>
#include <iostream>
#include <queue>
#include <string>
#include <fstream>
#include <sstream>
#include "ResourceController.h"
#include "ThreadController.h"
#include <unordered_map>

#pragma comment(lib, "Ws2_32.lib")

constexpr u_short BUFFER_SIZE	= 1024;
constexpr u_short MAX_PEERS		= 4;

struct PeerBuffer
{
	char buffer[BUFFER_SIZE];
	int length;

	PeerBuffer() : length(0)
	{
		memset(buffer, 0, sizeof(buffer));
	}
};

class NetworkController
{
private:
	std::shared_ptr<ThreadController> mThreadController = ThreadController::Instance();
	std::shared_ptr<ResourceController> mResourceController = ResourceController::Instance();

	std::unordered_map<SOCKET, PeerBuffer>	peerBuffers;
	std::queue<std::string>					messages;
	std::queue<std::string>					backlog;
	std::vector<SOCKET>						mPeers;


	SOCKET		mListenSocket;
	sockaddr_in mListenAddress;

	std::mutex mx;
    std::mutex addMx;
    std::mutex peerMx;

	//Private constructor for singleton pattern
	NetworkController();

	void HandleIncomingMessage(const SOCKET& peerSocket);
	void SendMessages(const std::queue<std::string> messages);
	void FindPeers();

	inline void ListenToPeer() {
        fd_set readSockets{};
		FD_ZERO(&readSockets);
        FD_SET(mListenSocket, &readSockets);

        while (true)
        {
            // Copy the socket set to prevent modification
            fd_set tempReadSockets = readSockets;

            // Wait for activity on any of the sockets
            int socketCount = select(0, &tempReadSockets, nullptr, nullptr, nullptr);
            if (socketCount == SOCKET_ERROR)
            {
                std::cerr << "select() failed" << std::endl;
                break;
            }

            // Process activity on the sockets
            for (int i = 0; i < socketCount; ++i)
            {
                const SOCKET& socket = tempReadSockets.fd_array[i];

                // Handle new connection
                if (socket == mListenSocket)
                {
                    // Accept a new connection
                    sockaddr_in clientAddr{};
                    int addrSize = sizeof(clientAddr);
                    SOCKET clientSocket = accept(mListenSocket, reinterpret_cast<sockaddr*>(&clientAddr), &addrSize);
                    if (clientSocket != INVALID_SOCKET)
                    {
                        if (PeerCount() > MAX_PEERS) {
                            OutputDebugString(L"Max number of peers reached. Connection rejected.");
                            closesocket(socket);
                            continue;
                        }
                        // Add the new client socket to the set
                        FD_SET(clientSocket, &readSockets);

                        {
                            // Add the new client socket to the vector of peer sockets
                            std::lock_guard<std::mutex> lock(peerMx);
                            mPeers.push_back(clientSocket);
                        }

                        OutputDebugString(L"New connection from: ");
                    }
                }
                else
                {
                    // Handle incoming message from each peer in a separate thread
                    mThreadController->AddTask([this, socket]() {this->HandleIncomingMessage(socket); }, TaskType::NETWORK);
                }
            }
        }
	}
    inline void ProcessBacklog() {
        while (true) {
            SendMessages(backlog);
        }
    }

public:
	static std::shared_ptr< NetworkController > Instance();

	~NetworkController();

	//Singleton pattern
	NetworkController(const NetworkController& NetworkController) = delete;
	NetworkController& operator=(NetworkController const&) = delete;

	void					InitWinSock();
	void					AddMessage(const std::string& pMessage);
	std::queue<std::string> ReadMessages();
	int						PeerCount();
};