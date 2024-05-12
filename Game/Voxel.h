#pragma once

constexpr unsigned int VOXEL_WIDTH = 2;// 12;
constexpr unsigned int VOXEL_HEIGHT = 2;// 12;

constexpr unsigned long VOXEL_AREA = VOXEL_WIDTH * VOXEL_HEIGHT;

class Cube;

struct GeneratorBufferData {
	glm::vec4	Colour;
	BOOL		IsTransparent;
	BOOL		IsInstanced;
};

/// <summary>
/// Helper class to manage a voxel. It works by specifying the generator data
/// needed to generate multiple instances of a cube and provides methods that
/// will update data in the generator.
/// </summary>
class VoxelCanvas
{
public:
	std::vector<GeneratorBufferData>* GetVoxel()
	{
		return &voxelGrid;
	}
public:
	VoxelCanvas()
	{
		voxelGrid = {};
		voxelGrid.resize(VOXEL_AREA);
	}

	unsigned long GetSize() { return voxelGrid.size() * sizeof(GeneratorBufferData); }

	GeneratorBufferData* GetVoxelData() { return voxelGrid.data(); }

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
	std::vector<GeneratorBufferData>	voxelGrid;
	std::mutex							mutex;
};

