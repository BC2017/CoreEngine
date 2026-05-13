#include "Engine/Renderer/SandboxFrameRenderer.hpp"
#include "Engine/RHI/Resource.hpp"
#include "Engine/Scene/Scene.hpp"
#include "TestHarness.hpp"

HFENGINE_TEST_CASE("unit.scene.ecs", "CreatesNamedEntityWithDefaultTransform")
{
    HFEngine::Scene::Scene scene;

    const HFEngine::Scene::EntityId entity = scene.CreateEntity("entity");

    HFENGINE_REQUIRE(entity.IsValid());
    HFENGINE_REQUIRE(scene.IsAlive(entity));
    HFENGINE_REQUIRE(scene.GetName(entity).has_value());
    HFENGINE_REQUIRE(scene.GetName(entity)->name == "entity");
    HFENGINE_REQUIRE(scene.GetTransform(entity).has_value());
    HFENGINE_REQUIRE(scene.GetTransform(entity)->scale.x == 1.0f);
    HFENGINE_REQUIRE(scene.GetTransform(entity)->scale.y == 1.0f);
    HFENGINE_REQUIRE(scene.GetTransform(entity)->scale.z == 1.0f);
}

HFENGINE_TEST_CASE("unit.scene.ecs", "ExtractsCameraAndMeshRenderData")
{
    HFEngine::Scene::Scene scene;

    const HFEngine::Scene::EntityId camera = scene.CreateEntity("camera");
    scene.SetCamera(camera, {});

    const HFEngine::Scene::EntityId mesh = scene.CreateEntity("mesh");
    HFEngine::Scene::TransformComponent transform;
    transform.position = { 1.0f, 2.0f, 3.0f };
    scene.SetTransform(mesh, transform);
    scene.SetMeshInstance(mesh, {
        {
            "draw",
            { 1, 1 },
            { 2, 1 },
            28,
            24,
            HFEngine::RHI::IndexFormat::Uint16,
            36,
        },
        { 5, 1 },
    });

    const HFEngine::Scene::SceneRenderData data = scene.BuildRenderData();

    HFENGINE_REQUIRE(data.hasPrimaryCamera);
    HFENGINE_REQUIRE(data.primaryCameraEntity == camera);
    HFENGINE_REQUIRE(data.meshDraws.size() == 1);
    HFENGINE_REQUIRE(data.meshDraws[0].entity == mesh);
    HFENGINE_REQUIRE(data.meshDraws[0].transform.position.x == 1.0f);
    HFENGINE_REQUIRE(HFEngine::RHI::ValidateDrawIndexedDesc(data.meshDraws[0].mesh.draw).valid);
}

HFENGINE_TEST_CASE("unit.scene.ecs", "DestroyedEntityIsNotExtracted")
{
    HFEngine::Scene::Scene scene;

    const HFEngine::Scene::EntityId mesh = scene.CreateEntity("mesh");
    scene.SetMeshInstance(mesh, {
        {
            "draw",
            { 1, 1 },
            { 2, 1 },
            28,
            24,
            HFEngine::RHI::IndexFormat::Uint16,
            36,
        },
        { 5, 1 },
    });
    scene.Destroy(mesh);

    HFENGINE_REQUIRE(!scene.IsAlive(mesh));
    HFENGINE_REQUIRE(scene.BuildRenderData().meshDraws.empty());
}

HFENGINE_TEST_CASE("unit.renderer.sandboxscene", "BuildsDefaultSandboxRenderData")
{
    const HFEngine::Scene::SceneRenderData data = HFEngine::Renderer::BuildSandboxSceneRenderData();

    HFENGINE_REQUIRE(data.hasPrimaryCamera);
    HFENGINE_REQUIRE(data.meshDraws.size() == 1);
    HFENGINE_REQUIRE(HFEngine::RHI::ValidateDrawIndexedDesc(data.meshDraws[0].mesh.draw).valid);
    HFENGINE_REQUIRE(data.meshDraws[0].mesh.pipeline.IsValid());
}
