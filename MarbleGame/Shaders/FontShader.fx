
struct VS_INPUT {
	float4 Position : POSITION;
	float2 TexCoord : TEXCOORD;
};

struct PS_INPUT {
	float4 Position : SV_Position;
	float2 TexCoord : TEXCOORD0;
};

cbuffer cbFontData : register(b0) {
	float fScreenWidth;
	float fScreenHeight;
	float fDestAngle;
}

Texture2D g_txFont : register(t0);

SamplerState g_samClamp : register(s0);
SamplerState g_samWrap : register(s1);
SamplerComparisonState g_samShadow : register(s2);

PS_INPUT VSMain(in VS_INPUT input) {
	PS_INPUT output;

	output.Position.x = input.Position.x * 2.0f / fScreenWidth - 1;
	output.Position.y = input.Position.y * 2.0f / -fScreenHeight + 1;
	output.Position.z = 0.0f;
	output.Position.w = 1.0f;
	output.TexCoord = input.TexCoord;

	return output;
}

float4 PSMain(in PS_INPUT input) : SV_Target{
	float4 color = g_txFont.Sample(g_samClamp, input.TexCoord);

	return float4(color.r * 1.0f, color.r * 1.0f, color.r * 1.0f, color.g);
}

