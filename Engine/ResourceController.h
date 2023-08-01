#pragma once
#include "windows.h"
#include <fstream>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <queue>
#include <vector>

struct Peer{
	std::string	ipAddress;
	u_short		port;
};

struct Config {
	std::map<std::string, Peer>	peers;
	u_short						port;
};

class ResourceController
{
protected:
	std::shared_ptr<Config> mConfig{};

	ResourceController();

public:
	~ResourceController();

	ResourceController(const ResourceController& pResourceController) = delete;
	ResourceController& operator=(ResourceController const&) = delete;

	const Config& const LoadConfig(const std::string& pFilename);
	const std::shared_ptr<Config> const getConfig() { return mConfig; }

	static std::shared_ptr< ResourceController > Instance();
};

