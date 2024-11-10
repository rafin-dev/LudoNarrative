#include "RootSignature.hlsl"

float3 color : register(b0);

[RootSignature(ROOTSIG)]
float4 main(float4 pos : SV_Position) : SV_Target
{   
    pos.x = pos.x / 1280;
    pos.y = pos.y / 720;
    return float4(pos.xy, 1.0f, 1.0f);
}