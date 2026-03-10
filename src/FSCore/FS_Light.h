#pragma once
struct Light
{
    XMFLOAT3 Strength = { 0.0f, 0.0f, 0.0f };
    float FalloffStart = 1.0f;                          // point/spot light only
    XMFLOAT3 Direction = { 0.0f, -1.0f, 0.0f };         // dir/spot light only
    float FalloffEnd = 10.0f;                           // point/spot light only
    XMFLOAT3 Position = { 0.0f, 0.0f, 0.0f };           // point/spot light only
    float SpotPower = 64.0f;                            // spot light only
};

#define LIGHT_AMBIANT_STRENGTH XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f)

struct PointLight : public Light
{
    PointLight()
    {
        //Used
        Position = { 0.0f, 5.0f, 0.0f };
        Strength = { 0.0f, 0.0f, 0.0f };
        FalloffStart = 1.0f;
        FalloffEnd = 10.0f;
        //Unused
		Direction = { 0.0f, 0.0f, 0.0f };
        SpotPower = 0.0f;
    }
};
struct DirectionalLight : public Light
{
    DirectionalLight()
    {
        //Used
        Position = { 0.0f, 5.0f, 0.0f };
        Strength = { 0.0f, 0.0f, 0.0f };
        Direction = { 0.0f, -1.0f, 0.0f };
        //Unused
		FalloffStart = 0.0f;
        FalloffEnd = 0.0f;
        SpotPower = 0.0f;
    }
};
struct SpotLight : public Light
{
    SpotLight()
    {
        //Used
        Position = { 0.0f, 5.0f, 0.0f };
        Strength = { 0.0f, 0.0f, 0.0f };
        Direction = { 0.0f, -1.0f, 0.0f };
        SpotPower = 64.0f;
        FalloffStart = 1.0f;
        FalloffEnd = 10.0f;
        //Unused
    }
};
