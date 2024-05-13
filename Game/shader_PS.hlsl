struct VOut
{
	float4 position : SV_POSITION;
	float4 colour : COLOR;
};

cbuffer ConstantBuffer
{
	matrix World;
	matrix View;
	matrix Projection;
	float4 Colour;
};

float4 main(VOut input) : SV_TARGET
{
	return input.colour * Colour;
}
