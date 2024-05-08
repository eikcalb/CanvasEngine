#pragma once
#include "Behavior.h"


class CameraBehavior: public Behavior
{
public:
	CameraBehavior(std::shared_ptr<GameObject> owner) : Behavior("CAMERA_CONTROLS", owner) {

	}

	virtual void OnMessage(Message* msg) override {

	}

	virtual void Update(double deltaTime) override;

	virtual void Reset() override;
};

