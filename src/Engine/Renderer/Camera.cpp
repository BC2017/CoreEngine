#include "Engine/Renderer/Camera.hpp"

namespace HFEngine::Renderer
{
    CameraMatrices BuildCameraMatrices(const CameraDesc& desc, float aspectRatio)
    {
        CameraMatrices matrices;
        matrices.view = Math::LookAtLH(desc.position, desc.target, desc.up);
        matrices.projection = Math::PerspectiveFovLH(desc.verticalFovRadians, aspectRatio, desc.nearPlane, desc.farPlane);
        matrices.viewProjection = Math::Multiply(matrices.view, matrices.projection);
        return matrices;
    }

    Math::Mat4 BuildSandboxViewProjection(std::uint32_t width, std::uint32_t height)
    {
        const float safeWidth = width == 0 ? 1.0f : static_cast<float>(width);
        const float safeHeight = height == 0 ? 1.0f : static_cast<float>(height);
        const float aspectRatio = safeWidth / safeHeight;
        return BuildCameraMatrices(CameraDesc{}, aspectRatio).viewProjection;
    }
}
