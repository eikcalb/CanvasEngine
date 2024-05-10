#pragma once
#include "Cube.h"
#include "Scene.h"

constexpr unsigned int VOXEL_WIDTH = 51;// 12;
constexpr unsigned int VOXEL_HEIGHT = 51;// 12;

class VoxelCanvas
{
public:
	void SetVoxel(int x, int y, std::shared_ptr<Cube> cube)
	{
		voxelGrid[y * VOXEL_WIDTH + x] = cube;
		cube->SetPosition(Vector4(x - VOXEL_WIDTH/2, y - VOXEL_HEIGHT/2));
	}

	Cube* GetVoxel(int x, int y)
	{
		return reinterpret_cast<Cube*>(voxelGrid[y * VOXEL_WIDTH + x].get());
	}
public:
	VoxelCanvas()
	{
		voxelGrid.resize(VOXEL_WIDTH * VOXEL_HEIGHT);
	}

	unsigned long GetSize() { return voxelGrid.size(); }

	void Reset() {
		for (int i = 0; i < (int)voxelGrid.size(); i++)
		{
			voxelGrid[i]->Reset();
		}
	};

private:
	std::vector<std::shared_ptr<Cube>>	voxelGrid;
	std::mutex							mutex;
};

