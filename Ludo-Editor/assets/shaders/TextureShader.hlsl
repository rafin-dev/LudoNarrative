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
    output.TilingFactor = input.TilingFactor;
    output.EntityID = input.EntityID;
    
    return output;
}

#kind pixel
#include "assets/shaders/Header.hlsli"

Texture2D<float4> Textures[32] : register(t0);
SamplerState Sampler : register(s0);

struct PS_OUTPUT
{
    float4 Color : SV_Target0;
    int EntityID : SV_Target1;
};

PS_OUTPUT main(VS_OUTPUT input)
{
    float4 texel = Textures[int(input.TexIndex)].Sample(Sampler, input.TexPos * input.TilingFactor);
    PS_OUTPUT output;
    output.Color = texel * input.Color;
    output.EntityID = input.EntityID;
    return output;
}