cbuffer PerFrameBuffer : register(b0)
{
	float4x4 viewProjection;
};

cbuffer PerMeshBuffer : register(b1)
{
	float4x4 world;
	float4x4 worldView;
	float4x4 worldViewInverseTranspose;
	float4 colour;
}

struct VertexIn
{
	float3 position : POSITION;
	float2 sphericalNormal : NORMAL;
	float2 texcoord : TEXCOORD;
	float4 colour : COLOR;
};

struct PixelIn
{
	float4 viewPosition : SV_POSITION;
	float3 worldPosition : POSITION;
	float3 viewNormal : NORMAL;
	float4 colour : COLOR;
	float2 texcoord : TEXCOORD0;
};

static const float Pi = 3.1415926535897932384626433832795;

float3 UnpackNormal(float2 sphericalNormal)
{
	// Unpack 0-1 to 0-pi
	float inclination = sphericalNormal.x * Pi;
	// Unpack 0-1 to -pi to pi
	float azimuth = (sphericalNormal.y * 2 - 1) * Pi;

	float si, ci;
	float sa, ca;
	sincos(inclination, si, ci);
	sincos(azimuth, sa, ca);

	return float3(si * ca, si * sa, ci);
}

PixelIn main(VertexIn input)
{
	PixelIn output;
	output.worldPosition = input.position;
	output.viewPosition = mul(world, float4(input.position, 1.0));
	output.viewPosition = mul(viewProjection, output.viewPosition);
	float3 normal = UnpackNormal(input.sphericalNormal);
	output.viewNormal = normalize(mul(normal, worldViewInverseTranspose));
	output.colour = input.colour * colour;
	output.texcoord = input.texcoord;
	return output;
}