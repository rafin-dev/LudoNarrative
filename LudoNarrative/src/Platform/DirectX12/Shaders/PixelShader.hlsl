#include "RootSignature.hlsl"

[RootSignature(ROOTSIG)]
float4 main( float4 pos : SV_Position ) : SV_Target
{   
    //pos.x = pos.x / size.x;
    //pos.y = pos.y / size.y;
    
    return float4(0.0f, 0.0f, 1.0f, 1.0f);
}