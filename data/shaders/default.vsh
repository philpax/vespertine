cbuffer PerFrameBuffer : register(b0)
{
	float4x4 viewProjection;
};

cbuffer PerMeshBuffer : register(b1)
{
	float4x4 world;
}

struct VertexIn
{
	float3 position : POSITION;
	float3 colour : COLOR;
	float3 normal : NORMAL;
};

struct PixelIn
{
	float4 position : SV_POSITION;
	float3 worldPosition : POSITION;
	float3 normal : NORMAL;
	float3 colour : COLOR;
};

PixelIn main(VertexIn input)
{
	PixelIn output;
	output.position = mul(world, float4(input.position, 1.0));
	output.worldPosition = output.position;
	output.position = mul(viewProjection, output.position);
	output.normal = input.normal;
	output.colour = input.colour;
	return output;
}