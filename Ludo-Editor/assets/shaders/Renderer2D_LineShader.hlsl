#kind vertex

struct VS_INPUT
{
    float3 Position : POSITION;
    float4 Color : Color;
    int EntityID : ENTITYID;
};

struct VS_OUPUT
{
    float4 Position : SV_Position;
    float4 Color : COLOR;
    int EntityID : ENTITYID;
};

cbuffer ViewProjection : register(b0)
{
    float4x4 ViewProjection;
};

VS_OUPUT main(VS_INPUT input)
{
    VS_OUPUT output;
    output.Position = mul(float4(input.Position.xyz, 1.0f), ViewProjection);
    output.Color = input.Color;
    output.EntityID = input.EntityID;
    
    return output;
}

#kind pixel

struct VS_OUPUT
{
    float4 Position : SV_Position;
    float4 Color : COLOR;
    int EntityID : ENTITYID;
};

struct PS_OUTPUT
{
    float4 PixelColor : SV_Target0;
    int PixelEntityID : SV_Target1;
};

PS_OUTPUT main(VS_OUPUT input)
{
    PS_OUTPUT output;
    output.PixelColor = input.Color;
    output.PixelEntityID = input.EntityID;
    
    return output;
}