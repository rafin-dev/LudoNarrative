// VS Input / Output

struct VS_INPUT
{
    float3 WorldPos : WORLDPOS;
    float3 LocalPos : LOCALPOS;
    float4 Color : COLOR;
    float Thickness : THICKNESS;
    float Fade : FADE;
    int EntityID : ENTITYID;
};

struct VS_OUTPUT
{
    float4 WorldPos : SV_Position;
    float2 LocalPos : LOCALPOS;
    float4 Color : COLOR0;
    float Thickness : THICKNESS;
    float Fade : FADE;
    int EntityID : ENTITYID;
};