cbuffer LightBuffer
{
	float4 lightColor;
	float3 lightPosition;
	float ambient;
};

cbuffer ColorBuffer
{
	float4 entityColor;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float3 pos : POSITION;
	float3 normal : NORMAL;
};


float4 main(PixelInputType input) : SV_TARGET
{
	float3 lightDirection = normalize(lightPosition - input.pos);
	float intensity = saturate(dot(input.normal, lightDirection));
	intensity = ambient + (1 - ambient)*intensity;
	float4 color = entityColor;
	color.xyz *= lightColor.xyz*intensity;
	return color;
}