float4x4 projectionMatrix;

Texture2D wavesTexture;
float2 wavesPosition;

SamplerState sState {
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

struct VS_INPUT {
	float4 Position : POSITION;
	float2 TexCoord : TEXCOORD;
};

struct PS_INPUT {
	float4 Position : SV_Position;
	float2 TexCoord : TEXCOORD0;
};

PS_INPUT VSMain(in VS_INPUT input) {
	PS_INPUT output;

	//input.Position.xy *= 1.0f / 4.0f;
	output.Position = mul(input.Position, projectionMatrix);
	output.TexCoord = input.TexCoord;

	return output;
}

float4 PSMain(in PS_INPUT input) : SV_Target{
	/*input.TexCoord *= 10.0f;
	input.TexCoord = (input.TexCoord - 0.5f) * 2.0f;
	input.TexCoord -= 7.5f;

	input.TexCoord *= (1.0f - wavesPosition.y);
	float4 color = wavesTexture.Sample(sState, input.TexCoord);// *8.0f + wavesPosition * 2.0f);

	input.TexCoord *= 10.0f;
	input.TexCoord = (input.TexCoord - 0.5f) * 2.0f;
	input.TexCoord -= 7.5f;

	input.TexCoord *= (1.0f - wavesPosition.y);

	float length = 1.0f - min(1.0f, sqrt(input.TexCoord.x * input.TexCoord.x + input.TexCoord.y * input.TexCoord.y));
	float4 color;
	color.r = sin((length * 3.14159265f) * 8.0f) / 2.0f + 0.5f;
	color.g = sin((length * 3.14159265f) * 8.0f) / 2.0f + 0.5f;
	color.b = 0.5f;
	color.a = 0.0f;

	color *= wavesPosition.x;

	float4 emptyColor = 0.5f;
	float4 color;
	input.TexCoord = (input.TexCoord - 0.5f) * 2.0f;
	//input.TexCoord -= 0.5f;
	input.TexCoord *= (4.0f - wavesPosition.x);

	float length = 1.0f - min(1.0f, sqrt(input.TexCoord.x * input.TexCoord.x + input.TexCoord.y * input.TexCoord.y));
	color.r = cos((length * 3.14159265f) * 8.0f) / 2.0f + 0.5f;
	color.g = sin((length * 3.14159265f) * 8.0f) / 2.0f + 0.5f;
	if (wavesPosition.x * 8.0f >= 8.0f && length > 0.5f) {
	color.r = 0.5f;
	color.g = 0.5f;
	}
	color.b = 1.0f;
	color.a = 0.0f;

	float alpha = 1.0f - length * wavesPosition.x;
	//alpha *= wavesPosition.x;

	float4 outColor = (1.0f - alpha) * color + alpha * emptyColor;*/

	float4 color = wavesTexture.Sample(sState, input.TexCoord *8.0f + wavesPosition * 2.0f);

	return float4(color.rgb, 1.0f);
}

RasterizerState disableCull {
	CullMode = NONE;
};

DepthStencilState depthDisable {
	DepthEnable = false;
};
