#pragma once
#include <memory>

///
// ScriptingController is responsible for loading and running scripts in the engine.
// These scripts are user defined logic that can be used to control various features
// in the engine.
//
// Currently, the goal is to suppor tonly LUA scripting in this engine. This will be
// used to manage configurations and possibly control gameplay.
///
class ScriptingController
{
private:
	ScriptingController() {}

public:

	~ScriptingController() {
		// TODO: Destroy LUA contexts
	}


	static std::shared_ptr<ScriptingController> Instance() {
		static std::shared_ptr<ScriptingController> instance{ new ScriptingController };
		return instance;
	}
};

