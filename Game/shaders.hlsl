
//static const int INSTANCE_COUNT = 262144
#define INSTANCE_COUNT 10

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
};

cbuffer ConstantBuffer
{
	matrix World;
	matrix View;
	matrix Projection;
	float4 Colour;
};

cbuffer InstanceBuffer
{
   float4 instances[INSTANCE_COUNT];
};


VOut VShader(float4 position : POSITION, float4 colour : COLOR, uint instanceID : SV_InstanceID)
{
	VOut output;

	//InstanceData id = instances[instanceID]

	output.position = mul( position, World );
    output.position = mul( output.position, View );
    output.position = mul( output.position, Projection );
	output.colour = colour;

	return output;
}


float4 PShader(VOut input) : SV_TARGET
{
	return input.colour * Colour;
}
