// VS Input / Output

struct VS_INPUT
{
    float3 Pos : POSITION;
    float2 TexPos : TEXCOORD;
};

struct VS_OUTPUT
{
    float4 Position : SV_Position;
    float2 TexPos : UV;
};