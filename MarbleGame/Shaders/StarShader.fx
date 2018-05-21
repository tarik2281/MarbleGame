
#include "ShaderHeader.hlsl"

struct VS_INPUT {
	float4 Position : POSITION;
	float3 Normal : NORMAL;
	float2 TexCoord : TEXCOORD;
	float4 Tangent : TANGENT;
	float4 WorldPos : WORLDPOSITION;
};

struct VS_OUTPUT {
	float4 Position : SV_POSITION;
	float Clip : SV_ClipDistance0;
	float4 Diffuse : COLOR0;
	float2 TexCoord : TEXCOORD0;
	float3 Normal : TEXCOORD1;
	float3 LightVec : TEXCOORD5;
	float3 SurfaceToCamera : TEXCOORD2;
	float4 ShadowCoord : TEXCOORD3;
	float3 WorldPos : TEXCOORD4;
};

VS_OUTPUT VSMain(in VS_INPUT input) {
	VS_OUTPUT output;

	//if (mirror)
	//input.Position.y *= -1.0f;
	float4 transformedPos = mul(input.Position, mModel);
		transformedPos.xyz *= input.WorldPos.w;
	transformedPos += float4(input.WorldPos.xyz, 0.0f);

	if (iShadowPass == 0) {
		output.ShadowCoord = mul(transformedPos, mShadowView);

		if (bMirror) {
			transformedPos.y *= -1.0f;
			transformedPos.y += 2.0f * fWaterHeight;
		}
		output.WorldPos = transformedPos.xyz;


		output.Position = mul(transformedPos, mViewProjection);
		//output.Position = mul(output.Position, shadowBias);


		float3 tNormal = mul(input.Normal, (float3x3)mModel);
			tNormal = normalize(tNormal);
		output.Normal = tNormal;

		float3 lightDirection = normalize(float3(-1.0f, 0.8f, -0.3f));
			output.LightVec = lightDirection;

		float diffuse = dot(tNormal, lightDirection);
		diffuse = max(diffuse, 0.0f);

		output.SurfaceToCamera = normalize(vCameraPosition.xyz - transformedPos.xyz);

		output.Diffuse = diffuse, 0.0f, 0.0f, 1.0f;
		output.TexCoord = input.TexCoord;
	}
	else {
		output.Position = mul(transformedPos, mShadowView);
		output.Position = mul(output.Position, mCascades[iShadowPass - 1]);
		output.Diffuse = 0.0f;
		output.TexCoord = 0.0f;
		output.Normal = 0.0f;
		output.LightVec = 0.0f;
		output.SurfaceToCamera = 0.0f;
		output.ShadowCoord = 0.0f;
		output.WorldPos = 0.0f;
	}

	output.Clip = dot(transformedPos, vClipPlane);

	return output;
}

float4 PSMain(VS_OUTPUT input) : SV_TARGET{
	float4 color = float4(1.0f, 1.0f, 0.0f, 1.0f);

	float3 reflVec = -reflect(input.LightVec, input.Normal);
	float cosAngle = max(0.0, dot(normalize(input.SurfaceToCamera), reflVec));
	float specular = pow(cosAngle, 3.0f);
	float4 specColor = color * specular;

		input.ShadowCoord /= input.ShadowCoord.w;
	input.ShadowCoord.z += 0.0005f;

	float2 texCoord;

	texCoord.x = (input.ShadowCoord.x) / 2.0f + 0.5f;
	texCoord.y = (-input.ShadowCoord.y) / 2.0f + 0.5f;


	/*float shadow = 1.0f;
	float specShadow = 1.0f;
	if (input.Diffuse.r > 0.0f) {
		float count = 0.0f;
		for (float x = -1.0f; x <= 1.0f; x += 1.0f) {
			for (float y = -1.0f; y <= 1.0f; y += 1.0f) {
				float depth = shadowMap.Sample(ShadowState, texCoord + float2(x / shadowResolution, y / shadowResolution)).r;
				if (input.ShadowCoord.z > depth) {
					count += 1.0f;
				}
			}
		}

		shadow = 1.0f - (count / 9.0f);
		specShadow = shadow;
		shadow += 0.5f;
		shadow = saturate(shadow);
	}*/
	float specShadow = 1.0f;
	float4 ambientColor = color * 0.25f;
		float4 diffuseColor = color * input.Diffuse.r;
		float4 specularColor = 1.0f * specular * specShadow;

		float alpha = input.WorldPos.y - fWaterHeight;
	alpha = clamp(alpha, -2.0f, 2.0f);
	alpha = abs(alpha) * 0.5f;

	float4 outColor = ambientColor + diffuseColor + specularColor;// (input.Diffuse.r > 0.05 ? specularColor : float4(0.0f, 0.0f, 0.0f, 0.0f));
		return float4((outColor/* * (input.Diffuse.r > 0.0f ? shadow : 1.0f)*/).rgb, alpha);// +(1.0f - objectColor);
}
