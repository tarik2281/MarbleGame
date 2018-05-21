
#include "ShaderHeader.hlsl"

Texture2D g_txRefractionMap : register(t0);
Texture2D g_txReflectionMap : register(t1);
Texture2D g_txWavesMap : register(t2);

struct VS_INPUT {
	float4 Position : POSITION;
	float2 TexCoord : TEXCOORD;
};

struct PS_INPUT {
	float4 Position : SV_Position;
	float2 TexCoord : TEXCOORD0;
	float4 Projected : TEXCOORD1;
	float4 WorldPosition : TEXCOORD2;
	float3 SurfaceToCamera : TEXCOORD3;
	float3 LightVec : TEXCOORD4;
};

PS_INPUT VSMain(in VS_INPUT input) {
	PS_INPUT output;

	float4 transformedPos = mul(input.Position, mModel);
	output.WorldPosition = transformedPos;

	output.Position = mul(transformedPos, mViewProjection);

	output.Projected = output.Position;

	output.TexCoord = input.TexCoord;

	output.SurfaceToCamera = normalize(vCameraPosition.xyz - transformedPos.xyz);
	output.LightVec = normalize(float3(-1.0f, 0.8f, -0.3f));

	return output;
}

float4 PSMain(PS_INPUT input) : SV_Target{
	input.Projected /= input.Projected.w;

	float2 projTexCoord;
	projTexCoord.x = (input.Projected.x) / 2.0f + 0.5f;
	projTexCoord.y = (-input.Projected.y) / 2.0f + 0.5f;

	float2 offset = float2(fElapsedTime * 0.12f, fElapsedTime * 0.06f);
	float4 bumpTexel = g_txWavesMap.Sample(g_samWrap, (input.TexCoord) * 8.0f + offset * 2.0f);
	//float4 bumpTexel = wavesTexture.Sample(wState, (input.TexCoord));
	float2 perturbation = 0.1f * (bumpTexel.rg - 0.5f) * 2.0f;

		float3 eyeVector = normalize(vCameraPosition - input.WorldPosition.xyz);
		float fresnel = dot(eyeVector, float3(0.0f, 1.0f, 0.0f));

	float3 normal = (bumpTexel.rgb - 0.5f) * 2.0f;
	//normal.x *= -1.0f;
	//normal.z *= -1.0f;
	float3 reflVec = -reflect(input.LightVec, normal);
		float cosAngle = max(0.0f, dot(normalize(input.SurfaceToCamera), reflVec));
	float specular = pow(cosAngle, 1.0f);

	float4 test = g_txRefractionMap.Sample(g_samClamp, projTexCoord);
		float4 color = g_txRefractionMap.Sample(g_samClamp, projTexCoord + perturbation * test.a);
		float4 reflectionColor = color.a * g_txReflectionMap.Sample(g_samClamp, projTexCoord + perturbation * test.a) + (1.0f - color.a) * color;
		float4 refractionColor = (1.0f - color.a) * color + (color.a) * float4(0.58f, 0.76f, 1.0f, 1.0f);

		//refractionColor = color;
	//reflectionColor = someTexture.Sample(sState, projTexCoord + perturbation * test.a);

		float4 outColor = (1.0f - fresnel) * reflectionColor + fresnel * refractionColor;// +specular;

		return float4(outColor.rgb, 1.0f);
}