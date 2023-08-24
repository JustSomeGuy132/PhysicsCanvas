struct VSOUT
{
    float4 pos : SV_Position;
    float3 colour : COLOR;
    
};

cbuffer CBuf
{
    matrix transform;
};

VSOUT main( float3 pos : POSITION , float3 colour: COLOR)
{
    VSOUT output;
    output.pos = mul(float4(pos, 1.0f), transform);
    output.colour = colour;
    return output;
}