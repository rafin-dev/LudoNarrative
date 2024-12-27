#kind vertex
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
    //output.Position = mul(mul(float4(input.Pos.xyz, 1.0f), ModelMatrix), ViewProjection);
    output.Position = mul(float4(input.Pos.xyz, 1.0f), ViewProjection);
    output.Color = input.Color;
    output.TexPos = input.TexPos;
    output.TexIndex = input.TexIndex;
    
    return output;
}

#kind pixel
#include "assets/shaders/Header.hlsli"

cbuffer Material : register(b0)
{
    float4 Color;
    float TilingFactor;
}

Texture2D<float4> textures[] : register(t0);
SamplerState Sampler : register(s0);

float4 main(VS_OUTPUT input) : SV_Target
{
    float4 texel = textures[int(input.TexIndex)].Sample(Sampler, input.TexPos);
    return texel * input.Color;
    //return float4(input.TexPos.xy, 0.0f, 1.0f);
    //int txIndex = int(input.TexIndex);
    //return textures[1].Sample(Sampler[1], input.TexPosa) * input.Color;
}