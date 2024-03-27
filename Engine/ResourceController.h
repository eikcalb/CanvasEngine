#pragma once
#include <iostream>
#include <fstream>
#include <list>
#include <windows.h>
#include <map>
#include <memory>
#include <queue>
#include <vector>
#include <string>
#include <wrl/client.h>

struct Peer{
	std::string	ipAddress;
	u_short		port;
};

struct Config {
	std::map<std::string, Peer>	peers;
	u_short						port;
};

static const char* CONFIG_ENV_VAR = "VOX_CONFIG_FILE";

class ResourceController
{
	//std::vector< std::pair< std::wstring, VBO*>> mGeometries{};
	//std::vector< std::pair< std::wstring, TextureObject*>> mTextures{};
	//std::vector< std::pair< std::wstring, ShaderObject*>> mShaders{};
protected:
	std::shared_ptr<Config> mConfig{};

	ResourceController();

public:
	~ResourceController();

	ResourceController(const ResourceController& pResourceController) = delete;
	ResourceController& operator=(ResourceController const&) = delete;

	const Config& const LoadConfig(const std::string& pFilename);
	const std::shared_ptr<Config> const getConfig() {
		if (!mConfig) {
			// Logic from Stackoverflow: https://stackoverflow.com/a/15916732
			char* buf = nullptr;
			size_t sz = 0;
			if (_dupenv_s(&buf, &sz, CONFIG_ENV_VAR) == 0 && buf != nullptr)
			{
				LoadConfig(buf);
				free(buf);
			}
			else {
				char modulePath[MAX_PATH];
				GetModuleFileNameA(NULL, modulePath, MAX_PATH);
				std::string executableDir = modulePath;
				size_t lastBackslash = executableDir.find_last_of("\\/");
				if (lastBackslash != std::string::npos) {
					executableDir = executableDir.substr(0, lastBackslash + 1);
				}
				std::string absolutePath = executableDir + "./config.ini";

				LoadConfig(absolutePath);
			}
		}
		return mConfig;
	}

	//VBO* const LoadGeometry(const Renderer* const pRenderer, const std::wstring& pFilename);
	//const TextureObject* const LoadTexture(const Renderer* const pRenderer, const std::wstring& pFilename);
	//const ShaderObject* const LoadShader(const Renderer* const pRenderer, const std::wstring& pFilename);

	static std::shared_ptr< ResourceController > Instance();
};

