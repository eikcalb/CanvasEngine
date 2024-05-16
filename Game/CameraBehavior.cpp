#include "CameraBehavior.h"

#include "Game.h"

void CameraBehavior::Update(double deltaTime) {
	if (!_owner->IsAlive()) {
		return;
	}

	float updateTime = 100 * deltaTime;
	const Game *game = Game::TheGame;
	const auto& renderer = game->GetRendererSystem()->GetRenderer();
	auto camPos = renderer->GetCameraPosition();
	auto ic = game->GetInputController();

	//if (ic->IsKeyPressed(KEYS::A)) {
	//	glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), updateTime, right);
	//	up = glm::vec3(rotationMatrix * glm::vec4(up, 0.0f));
	//	forward = glm::vec3(rotationMatrix * glm::vec4(forward, 0.0f));
	//}
	//else if (ic->IsKeyPressed(KEYS::D)) {
	//	glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), -updateTime, right);
	//	up = glm::vec3(rotationMatrix * glm::vec4(up, 0.0f));
	//	forward = glm::vec3(rotationMatrix * glm::vec4(forward, 0.0f));
	//}

	// Camera navigation is courtesy of ChatGPT.
	// https://chat.openai.com/share/d03c4216-a6f1-46ee-a10a-641bb76cb132
	if (ic->IsKeyPressed(KEYS::Up)) {
		camPos[0] += up * updateTime;
	}
	else if (ic->IsKeyPressed(KEYS::Down)) {
		camPos[0] -= up * updateTime;
	}

	if (ic->IsKeyPressed(KEYS::W)) {
		camPos[0] += forward * updateTime;
	}
	else if (ic->IsKeyPressed(KEYS::S)) {
		camPos[0] -= forward * updateTime;
	}

	if (ic->IsKeyPressed(KEYS::Right)) {
		camPos[0] += right * updateTime;
	}
	else if (ic->IsKeyPressed(KEYS::Left)) {
		camPos[0] -= right * updateTime;
	}

	camPos[1] = camPos[0] + glm::normalize(forward);

	renderer->SetCameraPosition(camPos);
}

void CameraBehavior::Reset() {
	forward = glm::vec3(0.0f, 0.0f, 1.0f);
	up = glm::vec3(0.0f, 1.0f, 0.0f);
	right = glm::vec3(1.0f, 0.0f, 0.0f);
	Game::TheGame->GetRendererSystem()->GetRenderer()->ResetCameraPosition();
}