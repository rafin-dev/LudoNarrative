cbuffer Material : register(b0)
{
    float4 Color;
}

float4 main( float4 pos : SV_Position ) : SV_Target
{
    return Color;
}