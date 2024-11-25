#include "RootSignature.hlsl"

cbuffer Material : register(b1)
{
    float4 Foo;
    float4 FlatColor;
}

[RootSignature(ROOTSIG)]
float4 main( float4 pos : SV_Position ) : SV_Target
{
    return FlatColor;
}