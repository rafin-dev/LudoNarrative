#kind vertex

cbuffer ViewProjection : register(b0)
{
    float4x4 ViewProjection;
};

cbuffer Model : register(b1)
{
    float4x4 ModelMatrix;
};

float4 main(float3 Pos : Position) : SV_Position
{   
    return mul(mul(float4(Pos.xyz, 1.0f), ModelMatrix), ViewProjection);
}

#kind pixel

cbuffer Material : register(b0)
{
    float4 Color;
}

float4 main() : SV_Target
{
    return Color;
}