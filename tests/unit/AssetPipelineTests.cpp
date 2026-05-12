#include "Engine/Asset/AssetRegistry.hpp"
#include "TestHarness.hpp"

#include <string_view>

HFENGINE_TEST_CASE("unit.asset.registry", "CreatesStableAssetIds")
{
    const HFEngine::Asset::AssetId first =
        HFEngine::Asset::MakeAssetId(HFEngine::Asset::AssetType::Mesh, "assets/meshes/triangle.gltf");
    const HFEngine::Asset::AssetId second =
        HFEngine::Asset::MakeAssetId(HFEngine::Asset::AssetType::Mesh, "assets/meshes/triangle.gltf");
    const HFEngine::Asset::AssetId differentType =
        HFEngine::Asset::MakeAssetId(HFEngine::Asset::AssetType::Material, "assets/meshes/triangle.gltf");

    HFENGINE_REQUIRE(first.IsValid());
    HFENGINE_REQUIRE(first == second);
    HFENGINE_REQUIRE(first.value != differentType.value);
}

HFENGINE_TEST_CASE("unit.asset.registry", "RegistersAndFindsAssetRecords")
{
    HFEngine::Asset::AssetRegistry registry;

    const HFEngine::Asset::AssetRecord record = registry.RegisterSource(
        HFEngine::Asset::AssetType::Texture,
        "assets/textures/albedo.ktx2",
        "cache/textures/albedo.hfac");

    const HFEngine::Asset::AssetRecord* found = registry.Find(record.id);

    HFENGINE_REQUIRE(registry.Count() == 1);
    HFENGINE_REQUIRE(found != nullptr);
    HFENGINE_REQUIRE(found->sourcePath == "assets/textures/albedo.ktx2");
    HFENGINE_REQUIRE(found->cookedPath == "cache/textures/albedo.hfac");
}

HFENGINE_TEST_CASE("unit.asset.registry", "TracksDependenciesBetweenRegisteredAssets")
{
    HFEngine::Asset::AssetRegistry registry;

    const HFEngine::Asset::AssetRecord material =
        registry.RegisterSource(HFEngine::Asset::AssetType::Material, "assets/materials/default.material");
    const HFEngine::Asset::AssetRecord texture =
        registry.RegisterSource(HFEngine::Asset::AssetType::Texture, "assets/textures/default.ktx2");

    HFENGINE_REQUIRE(registry.AddDependency(material.id, texture.id));

    const HFEngine::Asset::AssetRecord* found = registry.Find(material.id);

    HFENGINE_REQUIRE(found != nullptr);
    HFENGINE_REQUIRE(found->dependencies.size() == 1);
    HFENGINE_REQUIRE(found->dependencies[0] == texture.id);
}

HFENGINE_TEST_CASE("unit.asset.registry", "CreatesCookedAssetHeader")
{
    HFEngine::Asset::AssetRegistry registry;
    const HFEngine::Asset::AssetRecord record =
        registry.RegisterSource(HFEngine::Asset::AssetType::Scene, "assets/scenes/bootstrap.gltf");

    const HFEngine::Asset::CookedAssetHeader header = HFEngine::Asset::MakeCookedHeader(record);

    HFENGINE_REQUIRE(header.magic[0] == 'H');
    HFENGINE_REQUIRE(header.version == 1);
    HFENGINE_REQUIRE(header.type == HFEngine::Asset::AssetType::Scene);
    HFENGINE_REQUIRE(header.id == record.id);
    HFENGINE_REQUIRE(header.sourceHash == record.sourceHash);
    HFENGINE_REQUIRE(std::string_view(HFEngine::Asset::ToString(header.type)) == "Scene");
}
