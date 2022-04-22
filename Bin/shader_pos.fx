cbuffer BestBuffer : register(b0)
{
	float4x4 myToWorldMatrix;
	float4x4 myProjectionMatrix;
}

#define MAX_NUMBER_OF_DIRECTIONAL_LIGHTS 4
cbuffer BestBuffer : register(b1)
{
	float4 myToDirectionalLight[MAX_NUMBER_OF_DIRECTIONAL_LIGHTS];
}

struct VertexInput_Pos
{
	float4 position	: POSITION;
	float4 color	: COLOR;
	float4 normal	: NORMAL;
	float2 uv	: BAJS;
};

struct PixelInput
{
	float4 position : SV_POSITION;
	float4 color	: TEXCOORD0;
	float4 normal	: NORMAL;
	float2 uv	: TEXCOORD1;
};

struct PixelOutput
{
	float4 color : SV_TARGET;
};

SamplerState testState;

Texture2D shaderTextures : register(t0);

PixelInput VertexShader_Pos(VertexInput_Pos input)
{
	PixelInput output;
	output.position = input.position;
	output.position = mul(myToWorldMatrix, output.position);
	output.position = mul(myProjectionMatrix, output.position);
	
	output.normal = normalize(input.normal);
	output.normal = mul(myToWorldMatrix, output.normal);
	
	output.uv = input.uv;
	
	output.color = input.color;
	return output;
}

PixelOutput PixelShader_Pos(PixelInput input)
{
	PixelOutput output;
	float4 color = shaderTextures.Sample(testState, input.uv);
	
	float4 toLight = normalize(float4(-1, 1, -1, 0));
	float lambert = dot(input.normal, toLight);
	
	color *= lambert;
	output.color = float4(color.xyz, 1);
	
	
	
	return output;
}