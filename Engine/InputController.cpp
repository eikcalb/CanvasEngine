#include "InputController.h"

const std::string InputController::EVENT_KEY_INPUT = "input_key_event";
const std::string InputController::EVENT_MOUSE_INPUT = "input_mouse_event";

InputController::InputController(): ObserverSubject()
{
	// Apparently, it may not be possible to iterate through an enum with
	// non-continuous ordinal values.
	// Therefore, this doesn't work!!!!!
	// 
	//for (int i = 0; i <= static_cast<int>(KEYS::OemClear); ++i)
	//{
	//	KEYS key = static_cast<KEYS>(i);
	//	mKeyStates[key] = false;
	//}

	//for (int i = 0; i <= static_cast<int>(MOUSE_BUTTON::BACKWARD); ++i)
	//{
	//	MOUSE_BUTTON button = static_cast<MOUSE_BUTTON>(i);
	//	mMouseStates[button] = false;
	//}
}


bool RayIntersectsFrontFaceOfCube(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, const glm::vec3& cubePosition, float cubeSize) {
	// Compute the minimum and maximum X, Y, and Z values of the cube
	float minX = cubePosition.x - cubeSize / 2.0f;
	float maxX = cubePosition.x + cubeSize / 2.0f;
	float minY = cubePosition.y - cubeSize / 2.0f;
	float maxY = cubePosition.y + cubeSize / 2.0f;
	float minZ = cubePosition.z - cubeSize / 2.0f;
	float maxZ = cubePosition.z + cubeSize / 2.0f;

	// Check if the ray intersects with the plane of the front face of the cube
	float t = (maxZ - rayOrigin.z) / rayDirection.z;
	if (t < 0) {
		return false; // Ray is pointing away from the front face
	}

	// Calculate the intersection point on the plane
	float intersectionX = rayOrigin.x + t * rayDirection.x;
	float intersectionY = rayOrigin.y + t * rayDirection.y;

	// Check if the intersection point is within the bounds of the front face
	return (intersectionX >= minX && intersectionX <= maxX &&
		intersectionY >= minY && intersectionY <= maxY);
}

std::shared_ptr<InputController> InputController::Instance()
{
	static std::shared_ptr<InputController> instance{ new InputController() };
	return instance;
}
