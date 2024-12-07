#type vertex
#include "assets/shaders/Header.hlsli"

cbuffer ViewProjection : register(b0)
{
    float4x4 ViewProjection;
};

cbuffer Model : register(b1)
{
    float4x4 ModelMatrix;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    output.Position = mul(mul(float4(input.Pos.xyz, 1.0f), ModelMatrix), ViewProjection);
    output.TexPos = input.TexPos;
    return output;
}

#type pixel
#include "assets/shaders/Header.hlsli"

cbuffer Material : register(b0)
{
    float4 Color;
}

Texture2D Texture;
SamplerState Sampler;

float4 main(VS_OUTPUT input) : SV_Target
{
    return Texture.Sample(Sampler, input.TexPos);
}