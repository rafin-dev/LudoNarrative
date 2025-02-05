#kind vertex
#include "assets/shaders/Renderer2D_CircleHeader.hlsli"

cbuffer ViewProjection : register(b0)
{
    float4x4 ViewProjection;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    output.WorldPos = mul(float4(input.WorldPos.xyz, 1.0f), ViewProjection);
    output.LocalPos = float2(input.LocalPos.xy);
    output.Color = input.Color;
    output.Thickness = input.Thickness;
    output.Fade = input.Fade;
    output.EntityID = input.EntityID;
    
    return output;
}

#kind pixel
#include "assets/shaders/Renderer2D_CircleHeader.hlsli"

struct PS_OUTPUT
{
    float4 Color : SV_Target0;
    int EntityID : SV_Target1;
};

PS_OUTPUT main(VS_OUTPUT input)
{    
    float distance = 1.0f - length(input.LocalPos);
    float circle = smoothstep(0.0f, input.Fade, distance);
    circle *= smoothstep(input.Thickness + input.Fade, input.Thickness, distance);
    
    if (circle == 0.0f)
    {
        discard;
    }
    
    PS_OUTPUT output;
    output.Color = input.Color;
    output.Color.a *= circle;
    output.EntityID = input.EntityID;
    
    return output;
}