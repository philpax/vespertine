cbuffer PerFrameBuffer : register(b0)
{
	float4x4 viewProjection;
};

cbuffer PerMeshBuffer : register(b1)
{
	float4x4 world;
	float4x4 worldView;
	float4x4 worldViewInverseTranspose;
}

struct VertexIn
{
	float3 position : POSITION;
	float3 colour : COLOR;
	float3 normal : NORMAL;
	float2 texcoord : TEXCOORD;
};

struct PixelIn
{
	float4 position : SV_POSITION;
	float3 colour : COLOR;
	float3 normal : NORMAL;
	float2 texcoord : TEXCOORD0;
};

PixelIn main(VertexIn input)
{
	PixelIn output;
	output.position = mul(world, float4(input.position, 1.0));
	output.colour = input.colour;
	output.normal = input.normal;
	output.texcoord = input.texcoord;
	return output;
}