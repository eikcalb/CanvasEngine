#pragma once
#include "Behavior.h"

#include "GL\GLM\GLM.hpp"
#include "GL\GLM\GTC\matrix_transform.hpp"
#include "GL\GLM\GTC\type_ptr.hpp"

class CameraBehavior: public Behavior
{
private:
	glm::vec3 forward = glm::vec3(0.0f, 0.0f, 1.0f);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 right = glm::vec3(1.0f, 0.0f, 0.0f);

public:
	CameraBehavior(std::shared_ptr<GameObject> owner) : Behavior("CAMERA_CONTROLS", owner) {

	}

	virtual void OnMessage(Message* msg) override {

	}

	virtual void Update(double deltaTime) override;

	virtual void Reset() override;
};

