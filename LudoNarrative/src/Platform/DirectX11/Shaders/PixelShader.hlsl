#include "Header.hlsli"

cbuffer Material : register(b0)
{
    float4 Color;
}

Texture2D Texture;
SamplerState Sampler;

float4 main( VS_OUTPUT input ) : SV_Target
{
    return Texture.Sample(Sampler, input.TexPos);
}