#include "ResourceController.h"

ResourceController::ResourceController()
{
}

ResourceController::~ResourceController()
{
}

const Config& const ResourceController::LoadConfig(const std::string& pFilename) {
	Config iniData{};

	std::ifstream file(pFilename);
	if (!file) {
		OutputDebugString(L"Failed to open .ini file");
		return iniData;
	}

	std::string line;
	std::string currentSection;

	while (std::getline(file, line)) {
		// Skip empty lines and comments
		if (line.empty() || line[0] == ';') {
			continue;
		}

		// Check if it's a section header
		if (line[0] == '[' && line.back() == ']') {
			// Extract section name
			currentSection = line.substr(1, line.length() - 2);
		}
		else {
			// Parse key-value pairs
			size_t equalsPos = line.find('=');
			if (equalsPos != std::string::npos) {
				const std::string key = line.substr(0, equalsPos);
				std::string value = line.substr(equalsPos + 1);


				if (currentSection == "peers") {
					// `value` is in format "ip address":"port"
					const auto sepPos = value.find(':');
					const auto ip = value.substr(0, sepPos);
					const auto port = static_cast<u_short>(std::stoi(value.substr(sepPos + 1)));

					iniData.peers[key] = Peer{
						ip, port
					};
				}
				else {
					if (key == "port") {
						iniData.port = static_cast<u_short>(std::stoi(value));
					}
				}
			}
		}
	}

	file.close();

	mConfig = std::make_shared<Config>(iniData);
	return iniData;

}

std::shared_ptr<ResourceController> ResourceController::Instance()
{
	static std::shared_ptr<ResourceController> instance{ new ResourceController };
	return instance;
}