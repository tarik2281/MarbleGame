
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

Texture2D g_txBitmap : register(t0);

SamplerState g_samClamp : register(s0);
SamplerState g_samWrap : register(s1);
SamplerComparisonState g_samShadow : register(s2);

PS_INPUT VSMain(in VS_INPUT input) {
	PS_INPUT output;

	//output.Position = mul(input.Position, projectionMatrix);
	output.Position.x = input.Position.x * 2.0f / fScreenWidth - 1.0f;
	output.Position.y = input.Position.y * 2.0f / -fScreenHeight + 1.0f;
	output.Position.z = 1.0f;
	output.Position.w = 1.0f;
	output.TexCoord = input.TexCoord;

	return output;
}

float4 PSMain(in PS_INPUT input) : SV_Target{
	float4 color = g_txBitmap.Sample(g_samClamp, input.TexCoord);

	if (fDestAngle < 180.0f) {
		float2 dir = normalize((input.TexCoord - 0.5f) * 2.0f);
		float cosAngle = dot(dir, float2(0.0f, 1.0f));
		float angle = acos(cosAngle) * 180.0f / 3.14159265f;

		if (dir.x < 0.0f)
			return color;
		return color * (angle > fDestAngle && dir.x > 0.0f ? 1.0f : 0.5f);
	}
	else {
		float2 dir = normalize((input.TexCoord - 0.5f) * 2.0f);
			float cosAngle = dot(dir, float2(0.0f, -1.0f));
		float angle = acos(cosAngle) * 180.0f / 3.14159265f;

		if (dir.x > 0.0f)
			return color * 0.5f;
		return color * (angle > (fDestAngle - 180.0f) && dir.x < 0.0f ? 1.0f : 0.5f);
	}
}
