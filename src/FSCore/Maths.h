#pragma once

namespace FSC
{

    class Maths
    {
    public:

        static DirectX::XMFLOAT4X4 Identity4x4()
        {
            static DirectX::XMFLOAT4X4 I(
                1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f);

            return I;
        }

        static inline constexpr float Infinity = FLT_MAX;
        static inline constexpr float Pi = 3.1415926535f;

    };

}