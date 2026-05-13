#include "Engine/Scene/Scene.hpp"

#include "entt/entity/registry.hpp"

#include <utility>

namespace HFEngine::Scene
{
    namespace
    {
        [[nodiscard]] EntityId ToPublicEntity(entt::entity entity)
        {
            return { entt::to_integral(entity) + 1u };
        }

        [[nodiscard]] entt::entity ToEnttEntity(EntityId entity)
        {
            if (!entity.IsValid())
            {
                return entt::null;
            }

            return static_cast<entt::entity>(entity.value - 1u);
        }
    }

    struct Scene::Impl
    {
        entt::registry registry;
    };

    Scene::Scene()
        : impl_(std::make_unique<Impl>())
    {
    }

    Scene::~Scene() = default;
    Scene::Scene(Scene&&) noexcept = default;
    Scene& Scene::operator=(Scene&&) noexcept = default;

    EntityId Scene::CreateEntity(std::string name)
    {
        const entt::entity entity = impl_->registry.create();
        impl_->registry.emplace<TransformComponent>(entity);
        if (!name.empty())
        {
            impl_->registry.emplace<NameComponent>(entity, std::move(name));
        }

        return ToPublicEntity(entity);
    }

    bool Scene::IsAlive(EntityId entity) const
    {
        return impl_->registry.valid(ToEnttEntity(entity));
    }

    void Scene::Destroy(EntityId entity)
    {
        const entt::entity enttEntity = ToEnttEntity(entity);
        if (impl_->registry.valid(enttEntity))
        {
            impl_->registry.destroy(enttEntity);
        }
    }

    void Scene::SetTransform(EntityId entity, TransformComponent transform)
    {
        const entt::entity enttEntity = ToEnttEntity(entity);
        if (impl_->registry.valid(enttEntity))
        {
            impl_->registry.emplace_or_replace<TransformComponent>(enttEntity, transform);
        }
    }

    void Scene::SetCamera(EntityId entity, CameraComponent camera)
    {
        const entt::entity enttEntity = ToEnttEntity(entity);
        if (impl_->registry.valid(enttEntity))
        {
            impl_->registry.emplace_or_replace<CameraComponent>(enttEntity, camera);
        }
    }

    void Scene::SetMeshInstance(EntityId entity, MeshInstanceComponent mesh)
    {
        const entt::entity enttEntity = ToEnttEntity(entity);
        if (impl_->registry.valid(enttEntity))
        {
            impl_->registry.emplace_or_replace<MeshInstanceComponent>(enttEntity, std::move(mesh));
        }
    }

    std::optional<NameComponent> Scene::GetName(EntityId entity) const
    {
        const entt::entity enttEntity = ToEnttEntity(entity);
        if (!impl_->registry.valid(enttEntity))
        {
            return std::nullopt;
        }

        if (const NameComponent* name = impl_->registry.try_get<NameComponent>(enttEntity))
        {
            return *name;
        }

        return std::nullopt;
    }

    std::optional<TransformComponent> Scene::GetTransform(EntityId entity) const
    {
        const entt::entity enttEntity = ToEnttEntity(entity);
        if (!impl_->registry.valid(enttEntity))
        {
            return std::nullopt;
        }

        if (const TransformComponent* transform = impl_->registry.try_get<TransformComponent>(enttEntity))
        {
            return *transform;
        }

        return std::nullopt;
    }

    std::optional<CameraComponent> Scene::GetCamera(EntityId entity) const
    {
        const entt::entity enttEntity = ToEnttEntity(entity);
        if (!impl_->registry.valid(enttEntity))
        {
            return std::nullopt;
        }

        if (const CameraComponent* camera = impl_->registry.try_get<CameraComponent>(enttEntity))
        {
            return *camera;
        }

        return std::nullopt;
    }

    std::optional<MeshInstanceComponent> Scene::GetMeshInstance(EntityId entity) const
    {
        const entt::entity enttEntity = ToEnttEntity(entity);
        if (!impl_->registry.valid(enttEntity))
        {
            return std::nullopt;
        }

        if (const MeshInstanceComponent* mesh = impl_->registry.try_get<MeshInstanceComponent>(enttEntity))
        {
            return *mesh;
        }

        return std::nullopt;
    }

    SceneRenderData Scene::BuildRenderData() const
    {
        SceneRenderData data;

        const auto cameraView = impl_->registry.view<const CameraComponent>();
        for (const entt::entity entity : cameraView)
        {
            const CameraComponent& camera = cameraView.get<const CameraComponent>(entity);
            if (!data.hasPrimaryCamera || camera.primary)
            {
                data.hasPrimaryCamera = true;
                data.primaryCameraEntity = ToPublicEntity(entity);
                data.primaryCamera = camera;
                if (camera.primary)
                {
                    break;
                }
            }
        }

        const auto meshView = impl_->registry.view<const TransformComponent, const MeshInstanceComponent>();
        data.meshDraws.reserve(meshView.size_hint());
        for (const entt::entity entity : meshView)
        {
            const auto [transform, mesh] = meshView.get<const TransformComponent, const MeshInstanceComponent>(entity);
            data.meshDraws.push_back({ ToPublicEntity(entity), transform, mesh });
        }

        return data;
    }
}
