#pragma once

#include "Engine/Math/LinearAlgebra.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/RHI/Resource.hpp"

#include <cstdint>
#include <string>

namespace HFEngine::Scene
{
    struct EntityId
    {
        std::uint32_t value = 0;

        [[nodiscard]] constexpr bool IsValid() const noexcept
        {
            return value != 0;
        }

        [[nodiscard]] friend constexpr bool operator==(EntityId left, EntityId right) noexcept
        {
            return left.value == right.value;
        }
    };

    struct NameComponent
    {
        std::string name;
    };

    struct TransformComponent
    {
        Math::Vec3 position{ 0.0f, 0.0f, 0.0f };
        Math::Vec3 rotationEulerRadians{ 0.0f, 0.0f, 0.0f };
        Math::Vec3 scale{ 1.0f, 1.0f, 1.0f };
    };

    struct CameraComponent
    {
        Renderer::CameraDesc camera;
        bool primary = true;
    };

    struct MeshInstanceComponent
    {
        RHI::DrawIndexedDesc draw;
        RHI::GraphicsPipelineHandle pipeline;
    };

    struct SceneMeshDraw
    {
        EntityId entity;
        TransformComponent transform;
        MeshInstanceComponent mesh;
    };
}
