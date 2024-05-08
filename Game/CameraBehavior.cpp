#include "CameraBehavior.h"
#include "Game.h"

void CameraBehavior::Update(double deltaTime) {
	auto& game = Game::TheGame;
	const auto& renderer = game->GetRendererSystem()->GetRenderer();
	glm::vec3 camPos = renderer->GetCameraPosition();

	if (game->GetInputController()->IsKeyPressed(KEYS::OemComma)) {
		camPos += glm::vec3(0, 10 * deltaTime, 0);
	}
	else if (game->GetInputController()->IsKeyPressed(KEYS::OemPeriod)) {
		camPos += glm::vec3(0, 10 * -deltaTime, 0);
	}

	if (game->GetInputController()->IsKeyPressed(KEYS::Up)) {
		camPos += glm::vec3(0, 0, 10 * deltaTime);
	}
	else if (game->GetInputController()->IsKeyPressed(KEYS::Down)) {
		camPos += glm::vec3(0, 0, 10 * -deltaTime);
	}

	if (game->GetInputController()->IsKeyPressed(KEYS::Left)) {
		camPos += glm::vec3(10 * deltaTime, 0, 0);
	}
	else if (game->GetInputController()->IsKeyPressed(KEYS::Right)) {
		camPos += glm::vec3(10 * -deltaTime, 0, 0);
	}
	renderer->SetCameraPosition(camPos);
}

void CameraBehavior::Reset() {
	Game::TheGame->GetRendererSystem()->GetRenderer()->ResetCameraPosition();
}