//cbuffer Material : register(b1)
//{
//    float4 Foo;
//    float4 FlatColor;
//}

float4 main( float4 pos : SV_Position ) : SV_Target
{
    return float4(0.0f, 1.0f, 0.0f, 1.0f);
}