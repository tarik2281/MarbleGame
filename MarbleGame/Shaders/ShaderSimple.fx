
#include "ShaderHeader.hlsl"

Texture2D g_txDiffuse : register(t0);

struct VertexShaderInput {
	float4 Position : POSITION;
	float3 Normal : NORMAL;
	float2 TexCoord : TEXCOORD;
	//float4 Tangent : TANGENT;
};

struct VertexShaderOutput {
	float4 Position : SV_POSITION;
	float Clip : SV_ClipDistance0;
	float4 Diffuse : COLOR0;
	float2 TexCoord : TEXCOORD0;
	float3 LightVec : TEXCOORD1;
	float3 SurfaceToCamera : TEXCOORD2;
	float4 ShadowCoord : TEXCOORD3;
	float3 WorldPos : TEXCOORD4;
	float3 Normal : TEXCOORD5;
};

VertexShaderOutput VSMain(in VertexShaderInput input) {
	VertexShaderOutput output;

	float4 transformedPos = mul(input.Position, mModel);

	float3 tVertex = transformedPos.xyz;

	output.ShadowCoord = mul(transformedPos, mShadowView);

	if (bMirror) {
		transformedPos.y *= -1.0f;
		transformedPos.y += 2.0f * fWaterHeight;
	}
	output.WorldPos = transformedPos.xyz;

	output.Position = mul(transformedPos, mViewProjection);

	float3 tNormal = normalize(mul(input.Normal, (float3x3)mModel));
	output.Normal = tNormal;

	output.LightVec = normalize(float3(-1.0f, 0.8f, -0.3f));

	float diffuse = dot(tNormal, output.LightVec);
	diffuse = max(diffuse, 0.0f);

	output.SurfaceToCamera = normalize(vCameraPosition.xyz - tVertex);

	output.Diffuse = diffuse, 0.0f, 0.0f, 1.0f;
	output.TexCoord = input.TexCoord;

	output.Clip = dot(transformedPos, vClipPlane);

	return output;
}

float4 PSMain(VertexShaderOutput input) : SV_TARGET{
	float4 color = g_txDiffuse.Sample(g_samWrap, input.TexCoord);

	float3 reflVec = -reflect(input.LightVec, input.Normal);
	float cosAngle = max(0.0f, dot(normalize(input.SurfaceToCamera), reflVec));
	float specular = pow(cosAngle, 3.0f);

	input.ShadowCoord /= input.ShadowCoord.w;
	input.ShadowCoord.z += 0.0005f;

	float2 texCoord;
	texCoord.x = (input.ShadowCoord.x) / 2.0f + 0.5f;
	texCoord.y = (-input.ShadowCoord.y) / 2.0f + 0.5f;


	float4 ambientColor = color * 0.25f;
	float4 diffuseColor = color * input.Diffuse.r;
	float4 specColor = color * specular;

	float4 outColor = diffuseColor + ambientColor + (input.Diffuse.r > 0.05 ? specColor : float4(0.0f, 0.0f, 0.0f, 0.0f));
		return float4(outColor.rgb, vColor.a);
}
