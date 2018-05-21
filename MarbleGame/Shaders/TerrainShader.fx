
#include "ShaderHeader.hlsl"

Texture2D g_txDiffuse : register(t0);

struct VS_INPUT {
	float4 Position : POSITION;
	float2 TexCoord : TEXCOORD;
	float3 Normal : NORMAL;
};

struct VS_OUTPUT {
	float4 Position : SV_Position;
	float Clip : SV_ClipDistance0;
	float Diffuse : COLOR0;
	float2 TexCoord : TEXCOORD0;
	float3 WorldPos : TEXCOORD1;
	float4 ShadowCoord : TEXCOORD2;
	float vDepth : TEXCOORD3;
};

VS_OUTPUT VSMain(in VS_INPUT input) {
	VS_OUTPUT output;

	float4 transformedPos = mul(input.Position, mModel);
		output.ShadowCoord = mul(transformedPos, mShadowView);

	if (bMirror) {
		transformedPos.y *= -1.0f;
		transformedPos.y += 2.0f * fWaterHeight;
	}
		output.WorldPos = transformedPos.xyz;
	output.Position = mul(transformedPos, mView);
	output.vDepth = output.Position.z;
	output.Position = mul(output.Position, mProjection);

	output.TexCoord = input.TexCoord;

	float3 lightDir = normalize(float3(-1.0f, 0.8f, -0.3f));
	output.Diffuse = max(dot(lightDir, input.Normal), 0.0f);

	output.Clip = dot(transformedPos, vClipPlane);

	return output;
}

float4 PSMain(in VS_OUTPUT input) : SV_Target{
	float4 texColor = g_txDiffuse.Sample(g_samWrap, input.TexCoord * 5.0f);

	float4 ambientColor = texColor * 0.25f;
	float4 diffuseColor = texColor * input.Diffuse;

	int iCascadeIndex = 0;
	float4 vCurrentDepth = abs(input.vDepth);
		float4 fComparison = (vCurrentDepth > fCascadePartitionsFrustum);
		float fIndex = dot(vComparisonMask, fComparison);
	float numCascades = dot(vComparisonMask, vComparisonMask);
	iCascadeIndex = min(fIndex, numCascades);

	input.ShadowCoord = mul(input.ShadowCoord, mCascades[iCascadeIndex]);


	input.ShadowCoord /= input.ShadowCoord.w;
	input.ShadowCoord.z += 0.0005f;

	float2 texCoord;

	texCoord.x = (input.ShadowCoord.x) / 2.0f + 0.5f;
	texCoord.y = (-input.ShadowCoord.y) / 2.0f + 0.5f;
	texCoord.x /= numCascades;
	texCoord.x += (float)iCascadeIndex / 4.0f;

	float shadow = ShadowContribution(texCoord, input.ShadowCoord.z);
	shadow = saturate(shadow + 0.5f);

	float alpha = input.WorldPos.y - fWaterHeight;
	alpha = clamp(alpha, -3.25f, 3.25f);
	alpha = abs(alpha) / 4.0f;


	if (bVisualizeCascades) {
		float3 colors[4];
		colors[0] = float3(1.0f, 0.0f, 0.0f);
		colors[1] = float3(0.0f, 1.0f, 0.0f);
		colors[2] = float3(0.0f, 0.0f, 1.0f);
		colors[3] = float3(1.0f, 0.0f, 1.0f);

		float3 cascadeColor = colors[iCascadeIndex];

			return float4((ambientColor + diffuseColor).rgb * shadow * cascadeColor, alpha);
	}
	

	return float4((ambientColor + diffuseColor).rgb * shadow, alpha);
}
