#pragma once
#include "Scene.h"

#include "GL\GLM\GLM.hpp"
#include "GL\GLM\GTC\matrix_transform.hpp"

#include "NetworkController.h"
#include "NetworkMessage.h"
#include "Voxel.h"
#include "Utils.h"

constexpr int WIDTH = 512;
constexpr int HEIGHT = 512;

class Cube;

struct PeerData {
	Colour colour;
	long lastSequenceID;
	unsigned int mass[VOXEL_AREA];
	bool integrityPending;
};

class GamePlayScene :
	public Scene
{
	// Data
private:
	std::shared_ptr<Cube> _cube;
	glm::vec2 _lastMousePos;
	glm::vec3 _lastMouseRay;
	int _messageSendFreq;
	int _pendingIntegrityCount;

	// This will store the player information during the game.
	// For a start, we will fill in the initialization info.
	std::unordered_map<std::string, PeerData> peerDataMap;

	// Structors
public:
	GamePlayScene();
	virtual ~GamePlayScene();


	// Gets/Sets
private:
	void SetMousePos(const glm::vec2& mp) {
		_lastMousePos.x = mp.x;
		_lastMousePos.y = mp.y;
	}

	void SetMouseRay(const glm::vec3& mr) {
		_lastMouseRay.x = mr.x;
		_lastMouseRay.y = mr.y;
		_lastMouseRay.z = mr.y;
	}


	// Functions
public:

	// Setup
	virtual void Initialise() override;

	virtual void Start() override;

	virtual void End() override;

	/// Respond to input
	virtual void OnKeyboard(int key, bool down) override;
	
	/// Update current scene
	virtual void Update(double deltaTime) override;

	/// Render current scene
	virtual void Render(RenderSystem* renderer) override;

	virtual void OnMessage(Message* msg) override;

	std::string UpdatePeers();
	void HandleMessage(std::string peerID, const NetworkMessageContent& msg);
	void SendInit(std::shared_ptr<Connection> msg);

	void SendDraw(unsigned int x, unsigned int y);
	void SendIntegrity(unsigned int on);
	void SendIntegrity(std::string peerID, std::string);

	void GetPaint(unsigned int x, unsigned int y);

	// Reset the game
	void Reset();
};
