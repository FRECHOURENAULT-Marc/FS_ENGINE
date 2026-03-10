//***************************************************************************************
// texture.hlsl
// Affiche un objet texturé avec transformation world/view/proj
//***************************************************************************************

// Constant buffer pour l'objet 
cbuffer cbPerObject : register(b0)
{
    float4x4 gWorld; // Transformation objet -> monde
    float4 gMaskRect; // x, y, width, height en UV (0..1)
};

cbuffer cbPerMaterial : register(b2)
{
    float4 gColor;
    float4 gDiffuseAlbedo = { 1.0f, 1.0f, 1.0f, 1.0f };
    float3 gMetallic = { 0.01f, 0.01f, 0.01f };
    float gRoughness = 0.25f;
};

// Constant buffer pour la caméra 2D / frame
cbuffer cbPerFrame2D : register(b4)
{
    float4x4 gViewProj2D; // View * Projection
};

// Texture 
Texture2D gTexture : register(t0);

// Sampler
SamplerState gSampler : register(s0);

// Vertex input
struct VertexIn
{
    float3 PosL : POSITION; // position locale
    float2 Tex : TEXCOORD; // coordonnées UV
    float3 NormalL : NORMAL;
};

// Vertex output
struct VertexOut
{
    float4 PosH : SV_POSITION; // position homogène
    float2 Tex : TEXCOORD; // UV pour le pixel shader
    float3 PosW : POSITION;
    float3 NormalW : NORMAL;
};

VertexOut VS(VertexIn vin)
{
    VertexOut vout = (VertexOut) 0.0f;

    // Transformation vers l'espace monde
    float4 worldPos = mul(float4(vin.PosL, 1.0f), gWorld);
    vout.PosW = worldPos.xyz;

    // Assumes nonuniform scaling; otherwise, need to use inverse-transpose of world matrix.
    vout.NormalW = mul(vin.NormalL, (float3x3) gWorld);
    
    // Transformation vers clip space
    vout.PosH = mul(worldPos, gViewProj2D);

    // Passer les UV au pixel shader
    vout.Tex = vin.Tex;

    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
    // Apply rectangular mask in UV space (gMaskRect: x,y,width,height)
    float2 uv = pin.Tex;
    float2 minUV = gMaskRect.xy;
    float2 maxUV = gMaskRect.xy + gMaskRect.zw;

    // If outside mask, discard the pixel
    if (uv.x < minUV.x || uv.y < minUV.y || uv.x > maxUV.x || uv.y > maxUV.y)
        clip(-1);

    return gTexture.Sample(gSampler, uv);
}