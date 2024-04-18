#pragma once
#include "Cube.h"

constexpr int VOXEL_WIDTH = 512;
constexpr int VOXEL_HEIGHT = 512;

class VoxelCanvas
{
	// Constants
public:
	VoxelCanvas()
	{
		voxelGrid.resize(VOXEL_WIDTH * VOXEL_HEIGHT);
	}

	void SetVoxel(int x, int y, Cube* cube)
	{
		voxelGrid[y * VOXEL_WIDTH + x] = cube;
		cube->SetPosition(Vector4(x, y));
	}

	Cube* GetVoxel(int x, int y)
	{
		return voxelGrid[y * VOXEL_WIDTH + x];
	}
	void Update(double deltaTime) {};
	void Initialize() {};
	void Reset() {};

private:
	std::vector<Cube*> voxelGrid;
};

