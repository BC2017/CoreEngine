#include "Engine/Math/LinearAlgebra.hpp"

#include <cmath>

namespace HFEngine::Math
{
    Mat4 Mat4::Identity()
    {
        Mat4 matrix;
        matrix.At(0, 0) = 1.0f;
        matrix.At(1, 1) = 1.0f;
        matrix.At(2, 2) = 1.0f;
        matrix.At(3, 3) = 1.0f;
        return matrix;
    }

    float& Mat4::At(int row, int column)
    {
        return values[static_cast<std::size_t>((row * 4) + column)];
    }

    float Mat4::At(int row, int column) const
    {
        return values[static_cast<std::size_t>((row * 4) + column)];
    }

    const float* Mat4::Data() const
    {
        return values.data();
    }

    float* Mat4::Data()
    {
        return values.data();
    }

    Vec3 Add(Vec3 lhs, Vec3 rhs)
    {
        return {lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z};
    }

    Vec3 Subtract(Vec3 lhs, Vec3 rhs)
    {
        return {lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z};
    }

    Vec3 Scale(Vec3 value, float scalar)
    {
        return {value.x * scalar, value.y * scalar, value.z * scalar};
    }

    float Dot(Vec3 lhs, Vec3 rhs)
    {
        return (lhs.x * rhs.x) + (lhs.y * rhs.y) + (lhs.z * rhs.z);
    }

    Vec3 Cross(Vec3 lhs, Vec3 rhs)
    {
        return {
            (lhs.y * rhs.z) - (lhs.z * rhs.y),
            (lhs.z * rhs.x) - (lhs.x * rhs.z),
            (lhs.x * rhs.y) - (lhs.y * rhs.x)};
    }

    Vec3 Normalize(Vec3 value)
    {
        const float lengthSquared = Dot(value, value);
        if (lengthSquared <= 0.0f)
        {
            return {};
        }

        const float inverseLength = 1.0f / std::sqrt(lengthSquared);
        return Scale(value, inverseLength);
    }

    Mat4 Multiply(const Mat4& lhs, const Mat4& rhs)
    {
        Mat4 result;
        for (int row = 0; row < 4; ++row)
        {
            for (int column = 0; column < 4; ++column)
            {
                float value = 0.0f;
                for (int index = 0; index < 4; ++index)
                {
                    value += lhs.At(row, index) * rhs.At(index, column);
                }
                result.At(row, column) = value;
            }
        }
        return result;
    }

    Vec4 Transform(Vec4 value, const Mat4& matrix)
    {
        return {
            (value.x * matrix.At(0, 0)) + (value.y * matrix.At(1, 0)) + (value.z * matrix.At(2, 0)) + (value.w * matrix.At(3, 0)),
            (value.x * matrix.At(0, 1)) + (value.y * matrix.At(1, 1)) + (value.z * matrix.At(2, 1)) + (value.w * matrix.At(3, 1)),
            (value.x * matrix.At(0, 2)) + (value.y * matrix.At(1, 2)) + (value.z * matrix.At(2, 2)) + (value.w * matrix.At(3, 2)),
            (value.x * matrix.At(0, 3)) + (value.y * matrix.At(1, 3)) + (value.z * matrix.At(2, 3)) + (value.w * matrix.At(3, 3))};
    }

    Mat4 LookAtLH(Vec3 eye, Vec3 target, Vec3 up)
    {
        const Vec3 forward = Normalize(Subtract(target, eye));
        const Vec3 right = Normalize(Cross(up, forward));
        const Vec3 correctedUp = Cross(forward, right);

        Mat4 matrix = Mat4::Identity();
        matrix.At(0, 0) = right.x;
        matrix.At(1, 0) = right.y;
        matrix.At(2, 0) = right.z;

        matrix.At(0, 1) = correctedUp.x;
        matrix.At(1, 1) = correctedUp.y;
        matrix.At(2, 1) = correctedUp.z;

        matrix.At(0, 2) = forward.x;
        matrix.At(1, 2) = forward.y;
        matrix.At(2, 2) = forward.z;

        matrix.At(3, 0) = -Dot(right, eye);
        matrix.At(3, 1) = -Dot(correctedUp, eye);
        matrix.At(3, 2) = -Dot(forward, eye);
        return matrix;
    }

    Mat4 PerspectiveFovLH(float verticalFovRadians, float aspectRatio, float nearPlane, float farPlane)
    {
        const float yScale = 1.0f / std::tan(verticalFovRadians * 0.5f);
        const float xScale = yScale / aspectRatio;
        const float depthScale = farPlane / (farPlane - nearPlane);

        Mat4 matrix;
        matrix.At(0, 0) = xScale;
        matrix.At(1, 1) = yScale;
        matrix.At(2, 2) = depthScale;
        matrix.At(2, 3) = 1.0f;
        matrix.At(3, 2) = -nearPlane * depthScale;
        return matrix;
    }
}
