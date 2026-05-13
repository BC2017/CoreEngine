#pragma once

#include <array>

namespace HFEngine::Math
{
    constexpr float Pi = 3.14159265358979323846f;

    struct Vec3
    {
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;
    };

    struct Vec4
    {
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;
        float w = 0.0f;
    };

    struct Mat4
    {
        std::array<float, 16> values{};

        [[nodiscard]] static Mat4 Identity();
        [[nodiscard]] float& At(int row, int column);
        [[nodiscard]] float At(int row, int column) const;
        [[nodiscard]] const float* Data() const;
        [[nodiscard]] float* Data();
    };

    [[nodiscard]] Vec3 Add(Vec3 lhs, Vec3 rhs);
    [[nodiscard]] Vec3 Subtract(Vec3 lhs, Vec3 rhs);
    [[nodiscard]] Vec3 Scale(Vec3 value, float scalar);
    [[nodiscard]] float Dot(Vec3 lhs, Vec3 rhs);
    [[nodiscard]] Vec3 Cross(Vec3 lhs, Vec3 rhs);
    [[nodiscard]] Vec3 Normalize(Vec3 value);

    [[nodiscard]] Mat4 Multiply(const Mat4& lhs, const Mat4& rhs);
    [[nodiscard]] Vec4 Transform(Vec4 value, const Mat4& matrix);
    [[nodiscard]] Mat4 LookAtLH(Vec3 eye, Vec3 target, Vec3 up);
    [[nodiscard]] Mat4 PerspectiveFovLH(float verticalFovRadians, float aspectRatio, float nearPlane, float farPlane);
}
