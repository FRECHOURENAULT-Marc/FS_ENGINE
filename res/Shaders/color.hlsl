//**
// color.hlsl
// Affiche un objet textur avec transformation world/view/proj
//**

// Constant buffer pour l'objet
cbuffer cbPerObject : register(b0)
{
    float4x4 gWorld;
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

// Constant buffer pour la cam�ra / frame
cbuffer cbPerFrame : register(b1)
{
    float4x4 gViewProj; // View * Projection
};

// Vertex input
struct VertexIn
{
    float3 PosL : POSITION;
    float3 NormalL : NORMAL;
};

// Vertex output
struct VertexOut
{
    float4 PosH : SV_POSITION;
    float3 PosW : POSITION;
    float3 NormalW : NORMAL;
};

// Vertex shader
VertexOut VS(VertexIn vin)
{
    VertexOut vout = (VertexOut) 0.0f;
	
    // Transform to world space.
    float4 posW = mul(float4(vin.PosL, 1.0f), gWorld);
    vout.PosW = posW.xyz;

    // Assumes nonuniform scaling; otherwise, need to use inverse-transpose of world matrix.
    vout.NormalW = mul(vin.NormalL, (float3x3) gWorld);

    // Transform to homogeneous clip space.
    vout.PosH = mul(posW, gViewProj);

    return vout;
    //VertexOut vout;

    //// Transform to homogeneous clip space.
    //float4 pos = mul(float4(vin.PosL, 1.0f), gWorld);
    //vout.PosH = mul(pos, gViewProj);
    
    //return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
    // Interpolating normal can unnormalize it, so renormalize it.
    pin.NormalW = normalize(pin.NormalW);

    // Vector from point being lit to eye. 
    float3 toEyeW = normalize(gEyePosW.xyz - pin.PosW);

    // Combine gColor and gDiffuseAlbedo using tint factor (lerp between original and tinted)
    float4 tintedAlbedo = gDiffuseAlbedo * gColor;
    float4 baseAlbedo = gDiffuseAlbedo * tintedAlbedo;

    // Indirect lighting.
    float4 ambient = gAmbientLight * baseAlbedo;

    const float shininess = 1.0f - gRoughness;
    Material mat = { baseAlbedo, gMetallic, shininess };
    float shadowFactor = 1.0f;
    float4 directLight = ComputeLighting(dirLights, pointLights, spotLights, mat, pin.PosW,
        pin.NormalW, toEyeW, shadowFactor);

    float4 litColor = ambient + directLight;

    // Common convention to take alpha from diffuse material.
    litColor.a = baseAlbedo.a;

    return litColor;
    //return gColor;
}