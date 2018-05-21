
// per frame
cbuffer cbSceneData : register(b0) {
	matrix mProjection;
	matrix mView;
	matrix mViewProjection;
	float4 vCameraPosition;

	matrix mCascades[4];
	matrix mShadowView;
	float4 fCascadePartitionsFrustum;
	int numCascades;
	float fElapsedTime;
	float2 vShadowResolution;
	float4 vComparisonMask;
}

cbuffer cbRenderData : register(b1) {
	float4 vClipPlane;
	int iShadowPass;
	int bMirror;
	float fWaterHeight;
	int bVisualizeCascades;
}

cbuffer cbObjectData : register(b2) {
	matrix mModel;
	float4 vColor;
}


Texture2D g_txShadow : register(t6);

SamplerState g_samClamp : register(s0);
SamplerState g_samWrap : register(s1);
SamplerComparisonState g_samShadow : register(s2);

float ShadowContribution(float2 TexCoord, float depth) {
	float shadow = 0.0f;
	for (float x = -1.5f; x <= 1.5f; x += 1.0f) {
		for (float y = -1.5f; y <= 1.5f; y += 1.0f) {
			shadow += g_txShadow.SampleCmpLevelZero(g_samShadow, TexCoord + float2(x / vShadowResolution.x, y / vShadowResolution.y), depth);
		}
	}

	return shadow / 16.0f;

	//return g_txShadow.SampleCmpLevelZero(g_samShadow, TexCoord, depth);
}