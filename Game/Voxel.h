#pragma once
#include "Cube.h"
#include "Scene.h"

constexpr unsigned int VOXEL_WIDTH = 2;// 12;
constexpr unsigned int VOXEL_HEIGHT = 2;// 12;

class VoxelCanvas
{
public:
	void SetVoxel(int x, int y, std::shared_ptr<Cube> cube)
	{
		voxelGrid[y * VOXEL_WIDTH + x] = cube;
		cube->SetShouldUpdate(false);
		cube->SetPosition(Vector4(x, y));
	}

	Cube* GetVoxel(int x, int y)
	{
		return reinterpret_cast<Cube*>(voxelGrid[y * VOXEL_WIDTH + x].get());
	}
public:
	VoxelCanvas()
	{
		voxelGrid = {};
		voxelGrid.resize(VOXEL_WIDTH * VOXEL_HEIGHT);
	}

	unsigned long GetSize() { return voxelGrid.size(); }

	void Reset() {
		for (int i = 0; i < (int)voxelGrid.size(); i++)
		{
			voxelGrid[i]->SetShouldUpdate(false);
			auto y = std::floor(i/VOXEL_WIDTH);
			auto x = i % VOXEL_WIDTH;
			voxelGrid[i]->SetPosition(Vector4(x, y));
		}
	};

private:
	std::vector<std::shared_ptr<Cube>>	voxelGrid;
	std::mutex							mutex;
};

