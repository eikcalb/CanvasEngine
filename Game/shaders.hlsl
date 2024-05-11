#define INSTANCE_COUNT 262144

struct VOut
{
	float4 position : SV_POSITION;
	float4 colour : COLOR;
};

cbuffer InstanceBuffer : register(b1)
{
    float4 InstanceColors[INSTANCE_COUNT];
};

cbuffer ConstantBuffer : register( b0 )
{
	matrix World;
	matrix View;
	matrix Projection;
	float4 Colour;
};

VOut VShader(float4 position : POSITION, float4 colour : COLOR)
{
	VOut output;

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
