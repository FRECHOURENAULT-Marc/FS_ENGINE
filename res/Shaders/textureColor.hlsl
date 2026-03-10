//***************************************************************************************
// textureColor.hlsl
// Texture * Color shader
//***************************************************************************************

cbuffer cbPerObject : register(b0)
{
    float4x4 gWorld;
};

cbuffer cbPerFrame : register(b1)
{
    float4x4 gViewProj;
};

cbuffer cbPerMaterial : register(b2)
{
    float4 gColor;
};

Texture2D gTexture : register(t0);
SamplerState gSampler : register(s0);

struct VertexIn
{
    float3 PosL : POSITION;
    float2 Tex  : TEXCOORD;
};

struct VertexOut
{
    float4 PosH : SV_POSITION;
    float2 Tex  : TEXCOORD;
};

VertexOut VS(VertexIn vin)
{
    VertexOut vout;

    float4 worldPos = mul(float4(vin.PosL, 1.0f), gWorld);
    vout.PosH = mul(worldPos, gViewProj);
    vout.Tex = vin.Tex;

    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
    float4 texColor = gTexture.Sample(gSampler, pin.Tex);
    return texColor * gColor;
}