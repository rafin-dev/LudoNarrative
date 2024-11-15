#include "RootSignature.hlsl"

cbuffer Matrices : register(b0)
{
    float4x4 ViewProjection;
    float4x4 ModelMatrix;
}

[RootSignature(ROOTSIG)]
float4 main( float3 pos : Position) : SV_Position
{
    return mul(mul(float4(pos.xyz, 1.0), ModelMatrix), ViewProjection);
}