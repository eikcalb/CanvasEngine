#include "InputController.h"

const std::string InputController::EVENT_KEY_INPUT = "input_key_event";
const std::string InputController::EVENT_MOUSE_INPUT = "input_mouse_event";

InputController::InputController(): ObserverSubject()
{
	for (int i = 0; i <= static_cast<int>(KEYS::OemClear); ++i)
	{
		KEYS key = static_cast<KEYS>(i);
		mKeyStates[key] = false;
	}

	for (int i = 0; i <= static_cast<int>(MOUSE_BUTTON::BACKWARD); ++i)
	{
		MOUSE_BUTTON button = static_cast<MOUSE_BUTTON>(i);
		mMouseStates[button] = false;
	}
}

const int InputController::ScrollWheel() const
{
	return mMouseWheelValue;
}

const glm::vec2& InputController::MousePosition() const
{
	return mMousePosition;
}

// Get the ray direction in world space
// https://www.braynzarsoft.net/viewtutorial/q16390-20-picking
const MouseRay InputController::MouseRay(const glm::mat4& pViewInverse, const glm::mat4& pProjInverse, const float& width, const float& height)
{
	// Convert the screen coordinates to normalized device coordinates (NDC)
	float ndcX = (2.0f * mMousePosition.x) / width - 1.0f;
	float ndcY = 1.0f - (2.0f * mMousePosition.y) / height;

	auto clipSpacePosition = glm::vec4(ndcX, ndcY, 0.0f, 1.0f);

	//Multiply by inverse projection
	glm::vec4 position = pProjInverse * clipSpacePosition;

	// Convert view space position to world space
	position = pViewInverse * position;

	//Normalise to get direction of ray and return
	auto ray = glm::normalize(position - glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

	return ray;
}

std::shared_ptr<InputController> InputController::Instance()
{
	static std::shared_ptr<InputController> instance{ new InputController() };
	return instance;
}
