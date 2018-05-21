
#include "ShaderHeader.hlsl"

Texture2D g_txDiffuse : register(t0);
Texture2D g_txNormal : register(t1);

struct VS_INPUT {
	float4 Position : POSITION;
	float2 TexCoord : TEXCOORD;
	float3 Normal : NORMAL;
	float4 Tangent : TANGENT;
};

struct VS_OUTPUT {
	float4 Position : SV_POSITION;
	float Clip : SV_ClipDistance0;
	float Diffuse : COLOR0;
	float2 TexCoord : TEXCOORD0;
	float3 LightVec : TEXCOORD1;
	float3 SurfaceToCamera : TEXCOORD2;
	float4 ShadowCoord : TEXCOORD3;
	float3 WorldPos : TEXCOORD4;
	float vDepth : TEXCOORD5;
};

VS_OUTPUT VSMain(in VS_INPUT input) {
	VS_OUTPUT output;

	float4 transformedPos = mul(input.Position, mModel);

	if (iShadowPass == 0) {
		//if (mirror)
		//input.Position.y *= -1.0f;

		output.ShadowCoord = mul(transformedPos, mShadowView);
		//output.ShadowCoord = mul(output.ShadowCoord, mCascades[0]);

		if (bMirror) {
			transformedPos.y *= -1.0f;
			transformedPos.y += 2.0f * fWaterHeight;
		}

		output.WorldPos = transformedPos.xyz;

		output.Position = mul(transformedPos, mView);
		output.vDepth = output.Position.z;
		output.Position = mul(output.Position, mProjection);

		float3 tNormal = normalize(mul(input.Normal, (float3x3)mModel));
			float3 tTangent = normalize(mul(input.Tangent.rgb, (float3x3)mModel));
			float3 binormal = cross(tNormal, tTangent) * input.Tangent.w;

			float3x3 tbn = float3x3(tTangent.x, binormal.x, tNormal.x,
			tTangent.y, binormal.y, tNormal.y,
			tTangent.z, binormal.z, tNormal.z);

		float3 lightDirection = normalize(float3(-1.0f, 0.8f, -0.3f));
			float3 lightVec = normalize(mul(lightDirection, tbn));

			output.LightVec = lightVec;

		float diffuse = dot(normalize(tNormal), lightDirection);
		diffuse = max(diffuse, 0.0f);

		output.SurfaceToCamera = normalize(mul(vCameraPosition.xyz - transformedPos.xyz, tbn));

		output.Diffuse = diffuse;
		output.TexCoord = input.TexCoord;

		output.Clip = dot(transformedPos, vClipPlane);
	}
	else {
		output.Position = mul(transformedPos, mShadowView);
		output.Position = mul(output.Position, mCascades[iShadowPass - 1]);
		output.Diffuse = 0.0f;
		output.TexCoord = 0.0f;
		output.LightVec = 0.0f;
		output.SurfaceToCamera = 0.0f;
		output.ShadowCoord = 0.0f;
		output.WorldPos = 0.0f;
		output.Clip = 0.0f;
	}

	return output;
}



float4 PSMain(VS_OUTPUT input) : SV_Target{
	if (iShadowPass)
	return 0.0f;

	float4 color = g_txDiffuse.Sample(g_samWrap, input.TexCoord);
	float3 normal = g_txNormal.Sample(g_samWrap, input.TexCoord).rgb;

	normal = (normal - 0.5f) * 2.0f;
	
	float diffuse = dot(normal, input.LightVec);
	diffuse = max(diffuse, 0.0f);

	float3 reflVec = -reflect(input.LightVec, normal);
	float cosAngle = max(0.0, dot(normalize(input.SurfaceToCamera), reflVec));
	float specular = pow(cosAngle, 3.0f);
	float4 specColor = color * specular;


		int iCascadeIndex = 0;
	float4 vCurrentDepth = abs(input.vDepth);
		float4 fComparison = (vCurrentDepth > fCascadePartitionsFrustum);
		float fIndex = dot(vComparisonMask, fComparison);
	float numCascades = dot(vComparisonMask, vComparisonMask);
	iCascadeIndex = min(fIndex, numCascades);

	input.ShadowCoord = mul(input.ShadowCoord, mCascades[iCascadeIndex]);


		input.ShadowCoord /= input.ShadowCoord.w;
	input.ShadowCoord.z += 0.0005f;

	float2 shadowCoord;



	shadowCoord.x = (input.ShadowCoord.x) / 2.0f + 0.5f;
	shadowCoord.y = (- input.ShadowCoord.y) / 2.0f + 0.5f;
	shadowCoord.x /= numCascades;

	shadowCoord.x += (float)iCascadeIndex / 4.0f;

	
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
	//float shadow = min(1.0f, ShadowContribution(texCoord, input.ShadowCoord.z) + 0.5f);
	float shadow = ShadowContribution(shadowCoord, input.ShadowCoord.z);
	float specShadow = shadow;
	shadow += 0.5f;
	shadow = saturate(shadow);

	float4 ambientColor = color * 0.25f;
		float4 diffuseColor = color * (specShadow * diffuse + (1.0f - specShadow) * input.Diffuse.r);
		float4 specularColor = color * specular *specShadow;

		float alpha = input.WorldPos.y - fWaterHeight;
	alpha = clamp(alpha, -3.9f, 3.9f);
	alpha = abs(alpha) / 4.0f;



	float4 outColor = ambientColor + diffuseColor + (input.Diffuse.r > 0.05 ? specularColor : float4(0.0f, 0.0f, 0.0f, 0.0f));

	if (bVisualizeCascades) {
		float3 colors[4];
		colors[0] = float3(1.0f, 0.0f, 0.0f);
		colors[1] = float3(0.0f, 1.0f, 0.0f);
		colors[2] = float3(0.0f, 0.0f, 1.0f);
		colors[3] = float3(1.0f, 0.0f, 1.0f);

		float3 cascadeColor = colors[iCascadeIndex];

			outColor.rgb *= cascadeColor;
	}

#ifndef _EDITOR
		return float4((outColor * (input.Diffuse.r > 0.0f ? shadow : 1.0f)).rgb, alpha * vColor.a);
#else
		return float4((outColor * (input.Diffuse.r > 0.0f ? shadow : 1.0f)).rgb * (1.0f - objectColor.a) + objectColor.rgb * objectColor.a, alpha * opacity);
#endif
}
