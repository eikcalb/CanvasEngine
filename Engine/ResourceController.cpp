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
//
//const TextureObject* const ResourceController::LoadTexture(const Renderer* const pRenderer, const std::wstring& pFilename)
//{
//	auto hr{ S_OK };
//	//find and return from map
//	for (const auto& pair : mTextures)
//	{
//		if (pair.first == pFilename)
//		{
//			return pair.second;
//		}
//	}
//	//else create a new texture
//
//#ifdef  DIRECTX
//	TextureObject* newTexture = new TextureObject_DX();
//#elif OPENGL
//	TextureObject* newTexture = new TextureObject_GL();
//#endif
//	hr = newTexture->Create(pRenderer, pFilename);
//
//	if (FAILED(hr))
//	{
//		return nullptr;
//	}
//
//	//add it to the map
//	mTextures.emplace_back(make_pair(pFilename, newTexture));
//	//return the last thing in the dictionary (the newly created texture)
//	return mTextures.back().second;
//}
//
//
//VBO* const ResourceController::LoadGeometry(const Renderer* const pRenderer, const std::wstring& pFilename)
//{
//	auto hr{ S_OK };
//	//find and return from map
//	for (const auto& pair : mGeometries)
//	{
//		if (pair.first == pFilename)
//		{
//			return pair.second;
//		}
//	}
//	//else create a new geometry
//#ifdef  DIRECTX
//	VBO* newGeometry = new VBO_DX();
//#elif OPENGL
//	VBO* newGeometry = new VBO_GL();
//#endif
//	hr = newGeometry->Create(pRenderer, pFilename);
//	if (FAILED(hr))
//	{
//		return nullptr;
//	}
//	//add it to the map
//	mGeometries.emplace_back(make_pair(pFilename, newGeometry));
//	//return the last thing in the dictionary (the newly created geometry)
//	return mGeometries.back().second;
//}
//
//
//const ShaderObject* const ResourceController::LoadShader(const Renderer* const pRenderer, const std::wstring& pFilename)
//{
//	auto hr{ S_OK };
//	//find and return from map
//	for (const auto& pair : mShaders)
//	{
//		if (pair.first == pFilename)
//		{
//			return pair.second;
//		}
//	}
//	//else create a new shader
//#ifdef  DIRECTX
//	ShaderObject* newShader = new ShaderObject_DX();
//#elif OPENGL
//	ShaderObject* newShader = new ShaderObject_GL();
//#endif
//	hr = newShader->CreateVertex(pRenderer, pFilename, "VS", "vs_5_0");
//	if (FAILED(hr))
//	{
//		return nullptr;
//	}
//	hr = newShader->CreatePixel(pRenderer, pFilename, "PS", "ps_5_0");
//	if (FAILED(hr))
//	{
//		return nullptr;
//	}
//	//add it to the map
//	mShaders.emplace_back(make_pair(pFilename, newShader));
//	//return the last thing in the dictionary (the newly created shader)
//	return mShaders.back().second;
//}
//

std::shared_ptr<ResourceController> ResourceController::Instance()
{
	static std::shared_ptr<ResourceController> instance{ new ResourceController };
	return instance;
}