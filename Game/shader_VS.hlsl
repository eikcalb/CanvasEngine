#define WIDTH 512;
#define HEIGHT 512;

#pragma enable_d3d11_debug_symbols

struct VOut
{
	float4 position : SV_POSITION;
	float4 colour : COLOR;
};

struct InstanceData
{
	float4	Colour;
	bool	IsTransparent;
	bool	IsIntegrityCheck;
	// https://developer.nvidia.com/content/understanding-structured-buffer-performance
	float	_pad0;
	float	_pad1; // 32 bits
};

cbuffer ConstantBuffer
{
	matrix World;
	matrix View;
	matrix Projection;
	float4 Colour;
};

StructuredBuffer<InstanceData> InstanceBuffer : register(t0);

VOut main(float4 position : POSITION, float4 colour : COLOR, uint instanceID : SV_InstanceID)
{
	VOut output;

	InstanceData ib = InstanceBuffer[instanceID];

	// For positioning, we will check that the `instanceID` updated.
	// We will not position objects with a single instance, hence the
	// need to confirm we are at least at 0+1
	// We will calculate the next position based on the instance.
	float heightRatio = instanceID / WIDTH;
	uint y = floor(heightRatio);
	uint x = instanceID % WIDTH;
	position.x += x;
	position.y += y;
	

	output.position = mul( position, World );
	output.position = mul( output.position, View );
	output.position = mul( output.position, Projection );
	output.colour =  ib.Colour;

	if (ib.IsIntegrityCheck) {
		output.colour = float4(0.4f, 0.4f, 0.4f, 1.0f);
	}
	else if(ib.IsTransparent) {
		//output.position.y = -512 * 512 * 2;
		 output.colour = float4(0.0f, 0.0f, 0.0f, 0.0f);
	}

	return output;
}
