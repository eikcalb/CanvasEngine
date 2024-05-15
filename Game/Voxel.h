#pragma once
#define WIN32_LEAN_AND_MEAN // Required to prevent winsock/WinSock2 redifinition

#include <algorithm>
#include <mutex>
#include <vector>
#include <Windows.h>

// GLM
#include "GL\GLM\GLM.hpp"
#include "GL\GLM\GTC\matrix_transform.hpp"
#include "GL\GLM\GTC\type_ptr.hpp"

constexpr unsigned int VOXEL_WIDTH = 1;
constexpr unsigned int VOXEL_HEIGHT = 1;// 12;

constexpr unsigned long VOXEL_AREA = VOXEL_WIDTH * VOXEL_HEIGHT;

class Cube;

// Limit for buffers is 64k, hence we need a new way to move this into GPU.
struct GeneratorBufferData {
	glm::vec4	Colour;
	BOOL		IsTransparent = FALSE;
	BOOL		IsInstanced = TRUE;
	float		_pad0 = 0;
	float		_pad1 = 0;
};

/// <summary>
/// Helper class to manage a voxel. It works by specifying the generator data
/// needed to generate multiple instances of a cube and provides methods that
/// will update data in the generator.
/// </summary>
class VoxelCanvas
{
public:
	VoxelCanvas() {}
	
	~VoxelCanvas() {
		delete voxelGrid;
	}

	void Fill(GeneratorBufferData defaultValue)
	{
		std::lock_guard<std::mutex> lock(_mutex);
		std::fill(std::begin(voxelGrid), std::end(voxelGrid), defaultValue);
	}

	GeneratorBufferData& GetAt(unsigned long i) {
		return voxelGrid[i];
	}


	GeneratorBufferData& GetAt(unsigned int x, unsigned int y) {
		return voxelGrid[y * VOXEL_WIDTH + x];
	}


	void UpdateAt(unsigned long i, GeneratorBufferData data) {
		std::lock_guard<std::mutex> lock(_mutex);
		voxelGrid[i] = data;
	}

	void UpdateAt(unsigned int x, unsigned int y, GeneratorBufferData data) {
		std::lock_guard<std::mutex> lock(_mutex);
		voxelGrid[y * VOXEL_WIDTH + x] = data;
	}

	unsigned long GetSize() { return ARRAYSIZE(voxelGrid); }

	GeneratorBufferData* GetVoxelData() { return voxelGrid; }

	void Reset() {
		//for (int i = 0; i < (int)voxelGrid.size(); i++)
		//{
		//	voxelGrid[i]->SetShouldUpdate(false);
		//	auto y = std::floor(i/VOXEL_WIDTH);
		//	auto x = i % VOXEL_WIDTH;
		//	voxelGrid[i]->SetPosition(Vector4(x, y));
		//}
	};

private:
	GeneratorBufferData	voxelGrid[VOXEL_AREA];
	std::mutex			_mutex;
};

