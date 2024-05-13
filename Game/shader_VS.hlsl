struct VOut
{
	float4 position : SV_POSITION;
	float4 colour : COLOR;
};

struct InstanceData
{
	float4	Colour;
	bool	IsTransparent;
	bool	IsInstanced;
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

	InstanceData id = InstanceBuffer[instanceID];

	output.position = mul( position, World );
	output.position = mul( output.position, View );
	output.position = mul( output.position, Projection );
	output.colour = colour;

	return output;
}
