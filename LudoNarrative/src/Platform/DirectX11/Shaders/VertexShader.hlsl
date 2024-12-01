cbuffer ViewProjection : register(b0)
{
    float4x4 ViewProjection;
};

cbuffer Model : register(b1)
{
    float4x4 ModelMatrix;
};

float4 main( float3 pos : POSITION ) : SV_POSITION
{
    return mul(mul(float4(pos.xyz, 1.0f), ModelMatrix), ViewProjection);
}