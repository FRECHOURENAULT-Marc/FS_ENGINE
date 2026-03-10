#pragma once

struct ObjectConstants
{
	XMFLOAT4X4 m_World = Maths::Identity4x4();
};
struct ObjectConstants2D
{
    XMFLOAT4X4 m_World = Maths::Identity4x4();
    XMFLOAT4 m_Mask = { 0.0f, 0.0f, 10000.0f, 10000.0f };
};

struct MaterialConstants
{
	XMFLOAT4 m_Color;
    XMFLOAT4 DiffuseAlbedo = { 1.0f, 1.0f, 1.0f, 1.0f };
    XMFLOAT3 Metallic = { 0.01f, 0.01f, 0.01f };
    float Roughness = 0.25f;
};

struct CameraOC3D
{
    XMFLOAT4X4 m_ViewProj = Maths::Identity4x4();
};

struct CameraOC2D
{
    XMFLOAT4X4 m_ViewProj = Maths::Identity4x4();
};

#define MaxLights 16
struct PassConstants
{
    XMFLOAT4 EyePosW = { 0.0f, 0.0f, 0.0f, 1.0f };
    XMFLOAT4 AmbientLight = { 0.0f, 0.0f, 0.0f, 1.0f };

    Light dirLights[MaxLights];
    Light spotLights[MaxLights];
    Light pointLights[MaxLights];
};