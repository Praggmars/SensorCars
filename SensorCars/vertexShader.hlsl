cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix cameraMatrix;
};

struct VertexInputType
{
	float4 position : POSITION;
	float3 normal : NORMAL;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float3 pos : POSITION;
	float3 normal : NORMAL;
};


PixelInputType main(VertexInputType input)
{
	PixelInputType output;

	input.position.w = 1;
	output.position = mul(input.position, worldMatrix);
	output.pos = output.position.xyz;
	output.position = mul(output.position, cameraMatrix);
	output.normal = normalize(mul(input.normal, (float3x3)worldMatrix));

	return output;
}