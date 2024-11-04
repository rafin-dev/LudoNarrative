#include "RootSignature.hlsl"

[RootSignature(ROOTSIG)]
float4 main() : SV_Target
{
    float3 color = float3(1.0f, 0.0f, 0.0f);
	return float4(color, 1.0f);
}