#include "Engine/Renderer/FrameGraph.hpp"
#include "TestHarness.hpp"

HFENGINE_TEST_CASE("unit.renderer.frame_graph", "CompilesPassesInDependencyOrder")
{
    HFEngine::Renderer::FrameGraph graph;
    graph.AddPass({ "lighting", { "gbuffer" }, { "lit" } });
    graph.AddPass({ "gbuffer", {}, { "gbuffer" } });
    graph.AddPass({ "post", { "lit" }, { "backbuffer" } });

    const HFEngine::Renderer::CompiledFrameGraph compiled = graph.Compile();

    HFENGINE_REQUIRE(compiled.valid);
    HFENGINE_REQUIRE(compiled.executionOrder.size() == 3);
    HFENGINE_REQUIRE(compiled.executionOrder[0] == "gbuffer");
    HFENGINE_REQUIRE(compiled.executionOrder[1] == "lighting");
    HFENGINE_REQUIRE(compiled.executionOrder[2] == "post");
}

HFENGINE_TEST_CASE("unit.renderer.frame_graph", "RejectsDuplicatePassNames")
{
    HFEngine::Renderer::FrameGraph graph;
    graph.AddPass({ "depth", {}, { "depth" } });
    graph.AddPass({ "depth", {}, { "other" } });

    const HFEngine::Renderer::CompiledFrameGraph compiled = graph.Compile();

    HFENGINE_REQUIRE(!compiled.valid);
}

HFENGINE_TEST_CASE("unit.renderer.frame_graph", "RejectsMultipleWriters")
{
    HFEngine::Renderer::FrameGraph graph;
    graph.AddPass({ "a", {}, { "color" } });
    graph.AddPass({ "b", {}, { "color" } });

    const HFEngine::Renderer::CompiledFrameGraph compiled = graph.Compile();

    HFENGINE_REQUIRE(!compiled.valid);
}

HFENGINE_TEST_CASE("unit.renderer.frame_graph", "AllowsExternalReadResources")
{
    HFEngine::Renderer::FrameGraph graph;
    graph.AddPass({ "sample", { "external-texture" }, { "output" } });

    const HFEngine::Renderer::CompiledFrameGraph compiled = graph.Compile();

    HFENGINE_REQUIRE(compiled.valid);
    HFENGINE_REQUIRE(compiled.executionOrder[0] == "sample");
}
