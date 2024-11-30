float4 main( float3 pos : Position) : SV_Position
{
    //return mul(mul(float4(pos.xyz, 1.0), ModelMatrix), ViewProjection);
    return float4(pos.xyz, 1.0f);

}