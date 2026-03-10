//***************************************************************************************
// texture.hlsl
// Affiche un objet texturé avec transformation world/view/proj
//***************************************************************************************

// Constant buffer pour l'objet 
cbuffer cbPerObject : register(b0)
{
    float4x4 gWorld; // Transformation objet -> monde
};

// Constant buffer pour la caméra / frame
cbuffer cbPerFrame : register(b1)
{
    float4x4 gViewProj; // View * Projection
};

cbuffer cbPerMaterial : register(b2)
{
    float4 gColor;
    float4 gDiffuseAlbedo = { 1.0f, 1.0f, 1.0f, 1.0f };
    float3 gMetallic = { 0.01f, 0.01f, 0.01f };
    float gRoughness = 0.25f;
};

#include "LightingUtil.hlsl"
// Constant data that varies per material.
cbuffer cbPass : register(b3)
{
    float4 gEyePosW;
    float4 gAmbientLight;

    Light dirLights[MaxLights];
    Light spotLights[MaxLights];
    Light pointLights[MaxLights];
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
    vout.PosH = mul(worldPos, gViewProj);

    // Passer les UV au pixel shader
    vout.Tex = vin.Tex;

    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
    float4 diffuseAlbedo = gTexture.Sample(gSampler, pin.Tex) * gDiffuseAlbedo;
    
    // Interpolating normal can unnormalize it, so renormalize it.
    pin.NormalW = normalize(pin.NormalW);

    // Vector from point being lit to eye. 
    float3 toEyeW = normalize(gEyePosW.xyz - pin.PosW);

    // Indirect lighting.
    float4 ambient = gAmbientLight * diffuseAlbedo;

    const float shininess = 1.0f - gRoughness;
    Material mat = { diffuseAlbedo, gMetallic, shininess };
    float shadowFactor = 1.0f;
    float4 directLight = ComputeLighting(dirLights, pointLights, spotLights, mat, pin.PosW,
        pin.NormalW, toEyeW, shadowFactor);

    float4 litColor = ambient + directLight;

    // Common convention to take alpha from diffuse material.
    litColor.a = diffuseAlbedo.a;
    
    return litColor;
    //return gTexture.Sample(gSampler, pin.Tex);
}