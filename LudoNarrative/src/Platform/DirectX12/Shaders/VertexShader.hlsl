#include "RootSignature.hlsl"

float4x4 ViewProjection : register(b0);

[RootSignature(ROOTSIG)]
float4 main( float3 pos : Position) : SV_Position
{
    return mul(float4(pos.xyz, 1.0), ViewProjection);

}