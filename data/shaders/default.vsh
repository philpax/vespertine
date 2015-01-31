cbuffer VertexConstantBuffer : register(b0)
{
	float4x4 viewProjection;
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
	output.position = mul(viewProjection, float4(input.position, 1.0));
	output.colour = input.colour;
	return output;
}