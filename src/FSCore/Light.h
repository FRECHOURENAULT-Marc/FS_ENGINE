#pragma once

#define LIGHT_AMBIANT_STRENGTH XMFLOAT4(0.5f, 0.5f, 0.5f, 1.f)

namespace FSC
{

    struct Light
    {
        XMFLOAT3 m_Strength = { 0.0f, 0.0f, 0.0f };
        float m_FalloffStart = 1.0f;                          // point/spot light only
        XMFLOAT3 m_Direction = { 0.0f, -1.0f, 0.0f };         // dir/spot light only
        float m_FalloffEnd = 10.0f;                           // point/spot light only
        XMFLOAT3 m_Position = { 0.0f, 0.0f, 0.0f };           // point/spot light only
        float m_SpotPower = 64.0f;                            // spot light only
    };

    struct PointLight : public Light
    {
        PointLight()
        {
            //Used
            m_Position = { 0.0f, 5.0f, 0.0f };
            m_Strength = { 0.0f, 0.0f, 0.0f };
            m_FalloffStart = 1.0f;
            m_FalloffEnd = 10.0f;
            //Unused
            m_Direction = { 0.0f, 0.0f, 0.0f };
            m_SpotPower = 0.0f;
        }
    };
    struct DirectionalLight : public Light
    {
        DirectionalLight()
        {
            //Used
            m_Position = { 0.0f, 5.0f, 0.0f };
            m_Strength = { 0.0f, 0.0f, 0.0f };
            m_Direction = { 0.0f, -1.0f, 0.0f };
            //Unused
            m_FalloffStart = 0.0f;
            m_FalloffEnd = 0.0f;
            m_SpotPower = 0.0f;
        }
    };
    struct SpotLight : public Light
    {
        SpotLight()
        {
            //Used
            m_Position = { 0.0f, 5.0f, 0.0f };
            m_Strength = { 0.0f, 0.0f, 0.0f };
            m_Direction = { 0.0f, -1.0f, 0.0f };
            m_SpotPower = 64.0f;
            m_FalloffStart = 1.0f;
            m_FalloffEnd = 10.0f;
            //Unused
        }
    };

}