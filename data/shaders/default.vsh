cbuffer VertexConstantBuffer : register(b0)
{
	float4x4 model;
	float4x4 view;
	float4x4 projection;
};

struct VertexIn
{
	float3 position : POSITION;
	float3 colour : COLOR;
};

struct PixelIn
{
	float4 position : SV_POSITION;
	float3 colour : COLOR;
};

PixelIn main(VertexIn input)
{
	PixelIn output;
	output.position = float4(input.position, 1.0);
	output.position = mul(model, float4(input.position, 1.0));
	output.position = mul(view, output.position);
	output.position = mul(projection, output.position);
	output.colour = input.colour;
	return output;
}