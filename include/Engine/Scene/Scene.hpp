#pragma once

#include "Engine/Scene/Components.hpp"

#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace HFEngine::Scene
{
    struct SceneRenderData
    {
        bool hasPrimaryCamera = false;
        EntityId primaryCameraEntity;
        CameraComponent primaryCamera;
        std::vector<SceneMeshDraw> meshDraws;
    };

    class Scene
    {
    public:
        Scene();
        ~Scene();

        Scene(Scene&&) noexcept;
        Scene& operator=(Scene&&) noexcept;

        Scene(const Scene&) = delete;
        Scene& operator=(const Scene&) = delete;

        [[nodiscard]] EntityId CreateEntity(std::string name = {});
        [[nodiscard]] bool IsAlive(EntityId entity) const;
        void Destroy(EntityId entity);

        void SetTransform(EntityId entity, TransformComponent transform);
        void SetCamera(EntityId entity, CameraComponent camera);
        void SetMeshInstance(EntityId entity, MeshInstanceComponent mesh);

        [[nodiscard]] std::optional<NameComponent> GetName(EntityId entity) const;
        [[nodiscard]] std::optional<TransformComponent> GetTransform(EntityId entity) const;
        [[nodiscard]] std::optional<CameraComponent> GetCamera(EntityId entity) const;
        [[nodiscard]] std::optional<MeshInstanceComponent> GetMeshInstance(EntityId entity) const;
        [[nodiscard]] SceneRenderData BuildRenderData() const;

    private:
        struct Impl;
        std::unique_ptr<Impl> impl_;
    };
}
