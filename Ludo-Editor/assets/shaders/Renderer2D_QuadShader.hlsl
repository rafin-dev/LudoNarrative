#kind vertex
#include "assets/shaders/Renderer2D_QuadHeader.hlsli"

cbuffer ViewProjection : register(b0)
{
    float4x4 ViewProjection;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    output.Position = mul(float4(input.Pos.xyz, 1.0f), ViewProjection);
    output.Color = input.Color;
    output.TexPos = input.TexPos;
    output.TexIndex = input.TexIndex;
    output.TilingFactor = input.TilingFactor;
    output.EntityID = input.EntityID;
    
    return output;
}

#kind pixel
#include "assets/shaders/Renderer2D_QuadHeader.hlsli"

Texture2D Textures[32] : register(t0);
SamplerState Sampler : register(s0);

struct PS_OUTPUT
{
    float4 Color : SV_Target0;
    int EntityID : SV_Target1;
};

PS_OUTPUT main(VS_OUTPUT input)
{
    float4 texel = float4(1.0f, 1.0f, 1.0f, 1.0f);
    switch (input.TexIndex)
    {
        case 0: texel = Textures[0].Sample(Sampler, input.TexPos * input.TilingFactor); break;
        case 1: texel = Textures[1].Sample(Sampler, input.TexPos * input.TilingFactor); break;
        case 2: texel = Textures[2].Sample(Sampler, input.TexPos * input.TilingFactor); break;
        case 3: texel = Textures[3].Sample(Sampler, input.TexPos * input.TilingFactor); break;
        case 4: texel = Textures[4].Sample(Sampler, input.TexPos * input.TilingFactor); break;
        case 5: texel = Textures[5].Sample(Sampler, input.TexPos * input.TilingFactor); break;
        case 6: texel = Textures[6].Sample(Sampler, input.TexPos * input.TilingFactor); break;
        case 7: texel = Textures[7].Sample(Sampler, input.TexPos * input.TilingFactor); break;
        case 8: texel = Textures[8].Sample(Sampler, input.TexPos * input.TilingFactor); break;
        case 9: texel = Textures[9].Sample(Sampler, input.TexPos * input.TilingFactor); break;
        case 10: texel = Textures[10].Sample(Sampler, input.TexPos * input.TilingFactor); break;
        case 11: texel = Textures[11].Sample(Sampler, input.TexPos * input.TilingFactor); break;
        case 12: texel = Textures[12].Sample(Sampler, input.TexPos * input.TilingFactor); break;
        case 13: texel = Textures[13].Sample(Sampler, input.TexPos * input.TilingFactor); break;
        case 14: texel = Textures[14].Sample(Sampler, input.TexPos * input.TilingFactor); break;
        case 15: texel = Textures[15].Sample(Sampler, input.TexPos * input.TilingFactor); break;
        case 16: texel = Textures[16].Sample(Sampler, input.TexPos * input.TilingFactor); break;
        case 17: texel = Textures[17].Sample(Sampler, input.TexPos * input.TilingFactor); break;
        case 18: texel = Textures[18].Sample(Sampler, input.TexPos * input.TilingFactor); break;
        case 19: texel = Textures[19].Sample(Sampler, input.TexPos * input.TilingFactor); break;
        case 20: texel = Textures[20].Sample(Sampler, input.TexPos * input.TilingFactor); break;
        case 21: texel = Textures[21].Sample(Sampler, input.TexPos * input.TilingFactor); break;
        case 22: texel = Textures[22].Sample(Sampler, input.TexPos * input.TilingFactor); break;
        case 23: texel = Textures[23].Sample(Sampler, input.TexPos * input.TilingFactor); break;
        case 24: texel = Textures[24].Sample(Sampler, input.TexPos * input.TilingFactor); break;
        case 25: texel = Textures[25].Sample(Sampler, input.TexPos * input.TilingFactor); break;
        case 26: texel = Textures[26].Sample(Sampler, input.TexPos * input.TilingFactor); break;
        case 27: texel = Textures[27].Sample(Sampler, input.TexPos * input.TilingFactor); break;
        case 28: texel = Textures[28].Sample(Sampler, input.TexPos * input.TilingFactor); break;
        case 29: texel = Textures[29].Sample(Sampler, input.TexPos * input.TilingFactor); break;
        case 30: texel = Textures[30].Sample(Sampler, input.TexPos * input.TilingFactor); break;
        case 31: texel = Textures[31].Sample(Sampler, input.TexPos * input.TilingFactor); break;
    }
    PS_OUTPUT output;
    output.Color = texel * input.Color;
    output.EntityID = input.EntityID;
    return output;
}