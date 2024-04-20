#pragma once
#include <map>
#include <memory>
#include <semaphore>
#include <string>
#include <unordered_map>
#include <vector>

#include "Colour.h"
#include "Behavior.h"
#include "MathsHelper.h"
#include "Message.h"
#include "Observer.h"
#include "ObserverSubject.h"
#include "Vector4.h"


class Mesh;

typedef std::unordered_map<std::string, std::shared_ptr<Behavior>> BehaviorMap;

class GameObject : public ObserverSubject, public Observer
{
	// Render
protected:
	std::shared_ptr<Mesh>	_mesh;			// Vertex info
	Colour					_colour;		// Colour of object
	bool					_shouldDraw;	// Whether or not to draw


	// Data
protected:
	std::string				_type;			// Type of the object
	std::atomic<float>		_angle;			// Angle of object in degrees
	std::atomic<float>		_scale;			// Scale of the object (1 = normal)
	Vector4					_position;		// Position of object's origin
	std::atomic_bool		_alive;			// Alive flag; when false, is not updated
	std::atomic_bool		_deleteFlag;	// Set when you want this game object to be deleted by the game
	BehaviorMap				_behaviors;

	std::binary_semaphore	_semPosition;

	// Constructors
public:
	GameObject() = default;
	GameObject(std::string type);
	virtual ~GameObject();

	// Disable copy constructor + assignment operator
private:
	GameObject(const GameObject&);
	GameObject& operator=(const GameObject&) = default;


	// Gets/sets
public:
	std::shared_ptr<Mesh> GetMesh() const { return _mesh; }
	void SetMesh(std::shared_ptr<Mesh> m) { _mesh = m; }

	Colour GetColour() const { return _colour; }
	void SetColour(Colour c) { _colour = c; }

	bool ShouldDraw() const { return _shouldDraw; }
	void ShouldDraw(bool v) { _shouldDraw = v; }

	float GetAngle() const { return _angle; }
	void SetAngle(float v) { _angle = v; }

	float GetScale() const { return _scale; }
	void SetScale(float v) { _scale = v; }

	Vector4 GetPosition() const { return _position; }
	void SetPosition(Vector4 v) {
		_semPosition.acquire();
		_position = v;
		_semPosition.release();
	}

	bool IsAlive() const { return _alive; }
	void SetAlive(bool v) { _alive = v; }

	bool ShouldBeDeleted() const { return _deleteFlag; }
	void SetDeleteFlag(bool v) { _deleteFlag = v; }

	std::string GetType() const { return _type; }

	// Component Functions
public:
	bool AddBehavior(std::shared_ptr<Behavior> goc);
	bool RemoveBehavior(std::shared_ptr<Behavior> goc);
	bool RemoveBehavior(std::string componentType);
	std::shared_ptr<Behavior> GetBehavior(std::string tag);

	// General Functions
public:
	// Setup function -- called to initialise object and its components. Should only be called once
	virtual void Start();

	// Main update function (called every frame)
	virtual void Update(double deltaTime);

	// Shutdown function -- called when object is destroyed (i.e., from destructor)
	virtual void End();

	// Message handler (called when message occurs)
	virtual void OnMessage(Message* msg) override;

	// Resets the game object to the start state (similar to Start(), but may be called more than once)
	virtual void Reset();
};

