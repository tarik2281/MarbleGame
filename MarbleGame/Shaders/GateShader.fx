
#include "ShaderHeader.hlsl"

Texture2D g_txDiffuse : register(t0);

struct VS_INPUT {
	float4 Position : POSITION;
	float3 Normal : NORMAL;
	//float4 Tangent : TANGENT;
	float2 TexCoord : TEXCOORD;
};

struct VS_OUTPUT {
	float4 Position : SV_Position;
	float2 TexCoord : TEXCOORD0;
	float Clip : SV_ClipDistance0;
	float4 WorldPos : TEXCOORD1;
};

VS_OUTPUT VSMain(in VS_INPUT input) {
	VS_OUTPUT output;

	float4 transformedPos = mul(input.Position, mModel);

	if (bMirror) {
		transformedPos.y *= -1.0f;
		transformedPos.y += 2.0f * fWaterHeight;
	}

		output.Position = mul(transformedPos, mViewProjection);

	output.TexCoord = input.TexCoord;

	output.Clip = dot(transformedPos, vClipPlane);

	output.WorldPos = input.Position;

	return output;
}

float4 PSMain(VS_OUTPUT input) : SV_Target {
	float2 offset = float2(fElapsedTime * 0.66f, fElapsedTime * 0.33f);
	float4 texColor = g_txDiffuse.Sample(g_samWrap, input.TexCoord + offset); // +texPos.xy);

	float alpha = input.WorldPos.y - 1.5f;
	alpha = clamp(alpha, -0.5f, 0.5f);
	alpha = abs(alpha) / 0.5f;

#ifndef _EDITOR
	return float4(texColor.rgb, alpha * texColor.a);
#else
	return float4(texColor.rgb * (1.0f - objectColor.a) + objectColor.rgb * objectColor.a, alpha * texColor.a * (1.0f - objectColor.a) + objectColor.a);
#endif
}
