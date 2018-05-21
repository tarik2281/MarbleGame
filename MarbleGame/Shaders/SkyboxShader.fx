
#include "ShaderHeader.hlsl"

Texture2DArray g_txDiffuse : register(t0);

struct VS_INPUT {
	float4 Position : POSITION;
	float3 TexCoord : TEXCOORD;
};

struct PS_INPUT {
	float4 Position : SV_Position;
	float3 TexCoord : TEXCOORD0;
};

PS_INPUT VSMain(in VS_INPUT input) {
	PS_INPUT output;

	if (bMirror)
		input.Position.y *= -1.0f;
	
	output.Position.xyz = mul(input.Position.xyz, (float3x3)mView);
	output.Position.w = 1.0f;
	output.Position = mul(output.Position, mProjection);

	output.TexCoord = input.TexCoord;

	return output;
}

float4 PSMain(PS_INPUT input) : SV_Target{
	float4 color = g_txDiffuse.Sample(g_samClamp, input.TexCoord);
	color.a = 1.0f;
	return color;
}
