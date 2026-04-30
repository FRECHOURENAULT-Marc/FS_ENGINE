#pragma once

#define MaxLights 16

namespace FSC
{

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
        XMFLOAT4 m_DiffuseAlbedo = { 1.0f, 1.0f, 1.0f, 1.0f };
        XMFLOAT3 m_Metallic = { 0.01f, 0.01f, 0.01f };
        float m_Roughness = 0.25f;
    };

    struct CameraOC3D
    {
        XMFLOAT4X4 m_ViewProj = Maths::Identity4x4();
    };
    struct CameraOC2D
    {
        XMFLOAT4X4 m_ViewProj = Maths::Identity4x4();
    };

    struct PassConstants
    {
        XMFLOAT4 m_EyePosW = { 0.0f, 0.0f, 0.0f, 1.0f };
        XMFLOAT4 m_AmbientLight = { 0.0f, 0.0f, 0.0f, 1.0f };

        Light m_DirLights[MaxLights];
        Light m_SpotLights[MaxLights];
        Light m_PointLights[MaxLights];
    };

}