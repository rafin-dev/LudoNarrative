// VS Input / Output

struct VS_INPUT
{
    float3 Pos : POSITION;
    float4 Color : COLOR;
    float2 TexPos : TEXCOORD;
    float TexIndex : TexIndex;
    float TilingFactor : TilingFactor;
    int EntityID : ENTITYID;
};

struct VS_OUTPUT
{
    float4 Position : SV_Position;
    float4 Color : COLOR0;
    float2 TexPos : UV;
    float TexIndex : TEXCOORD;
    float TilingFactor : TilingFactor;
    int EntityID : ENTITYID;
};