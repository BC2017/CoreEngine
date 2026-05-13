#include "Engine/Math/LinearAlgebra.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "TestHarness.hpp"

#include <cmath>

namespace
{
    bool Approx(float lhs, float rhs, float epsilon = 0.0001f)
    {
        return std::fabs(lhs - rhs) <= epsilon;
    }
}

HFENGINE_TEST_CASE("unit.math.projection", "PerspectiveMapsDepthIntoClipRange")
{
    using namespace HFEngine;

    const Math::Mat4 projection = Math::PerspectiveFovLH(Math::Pi / 2.0f, 1.0f, 0.1f, 10.0f);
    const Math::Vec4 nearPoint = Math::Transform({0.0f, 0.0f, 0.1f, 1.0f}, projection);
    const Math::Vec4 farPoint = Math::Transform({0.0f, 0.0f, 10.0f, 1.0f}, projection);

    HFENGINE_REQUIRE(Approx(nearPoint.z / nearPoint.w, 0.0f));
    HFENGINE_REQUIRE(Approx(farPoint.z / farPoint.w, 1.0f));
}

HFENGINE_TEST_CASE("unit.math.camera", "LookAtMovesTargetInFrontOfCamera")
{
    using namespace HFEngine;

    const Math::Mat4 view = Math::LookAtLH({0.0f, 0.0f, -3.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f});
    const Math::Vec4 originInView = Math::Transform({0.0f, 0.0f, 0.0f, 1.0f}, view);
    HFENGINE_REQUIRE(Approx(originInView.z, 3.0f));
}

HFENGINE_TEST_CASE("unit.renderer.camera", "SandboxCameraProducesVisibleHomogeneousPosition")
{
    using namespace HFEngine;

    const Math::Mat4 sandboxViewProjection = Renderer::BuildSandboxViewProjection(1280, 720);
    const Math::Vec4 projectedOrigin = Math::Transform({0.0f, 0.0f, 0.0f, 1.0f}, sandboxViewProjection);
    HFENGINE_REQUIRE(projectedOrigin.w > 0.0f);
}
