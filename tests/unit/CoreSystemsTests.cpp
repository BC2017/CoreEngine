#include "Engine/Core/Assert.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/FileSystem.hpp"
#include "Engine/Core/Handle.hpp"
#include "Engine/Core/JobSystem.hpp"
#include "Engine/Core/Logger.hpp"
#include "TestHarness.hpp"

#include <filesystem>
#include <string_view>
#include <vector>

HFENGINE_TEST_CASE("unit.core.logging", "MemorySinkStoresEntriesInOrder")
{
    HFEngine::Core::MemoryLogSink sink;

    sink.Write({ HFEngine::Core::LogLevel::Info, "test", "first" });
    sink.Write({ HFEngine::Core::LogLevel::Warning, "test", "second" });

    HFENGINE_REQUIRE(sink.Entries().size() == 2);
    HFENGINE_REQUIRE(sink.Entries()[0].message == "first");
    HFENGINE_REQUIRE(sink.Entries()[1].level == HFEngine::Core::LogLevel::Warning);
    HFENGINE_REQUIRE(std::string_view(HFEngine::Core::ToString(HFEngine::Core::LogLevel::Fatal)) == "Fatal");
}

HFENGINE_TEST_CASE("unit.core.assert", "CheckReportsFailureMessage")
{
    const HFEngine::Core::CheckResult result =
        HFEngine::Core::Check(false, "value != 0", "zero is invalid");

    HFENGINE_REQUIRE(!result.passed);
    HFENGINE_REQUIRE(result.message.find("value != 0") != std::string::npos);
    HFENGINE_REQUIRE(result.message.find("zero is invalid") != std::string::npos);
}

HFENGINE_TEST_CASE("unit.core.clock", "FixedStepClockAdvancesDeterministically")
{
    HFEngine::Core::FixedStepClock clock(0.5);

    const HFEngine::Core::FrameTime first = clock.Tick();
    const HFEngine::Core::FrameTime second = clock.Tick();

    HFENGINE_REQUIRE(first.frameIndex == 1);
    HFENGINE_REQUIRE(second.frameIndex == 2);
    HFENGINE_REQUIRE(second.totalSeconds == 1.0);
}

HFENGINE_TEST_CASE("unit.core.files", "VirtualFileSystemResolvesMountedRelativePath")
{
    HFEngine::Core::VirtualFileSystem vfs;
    vfs.Mount("assets", std::filesystem::current_path());

    const std::optional<std::filesystem::path> path = vfs.Resolve("assets", "textures/test.ktx2");

    HFENGINE_REQUIRE(vfs.IsMounted("assets"));
    HFENGINE_REQUIRE(path.has_value());
    HFENGINE_REQUIRE(path->filename() == "test.ktx2");
    HFENGINE_REQUIRE(!vfs.Resolve("assets", "C:/absolute/path.txt").has_value());
}

HFENGINE_TEST_CASE("unit.core.handles", "HandleValidityUsesGeneration")
{
    struct TextureTag;

    HFEngine::Core::Handle<TextureTag> invalid;
    HFEngine::Core::Handle<TextureTag> valid{ 2, 1 };
    HFEngine::Core::Handle<TextureTag> same{ 2, 1 };

    HFENGINE_REQUIRE(!invalid.IsValid());
    HFENGINE_REQUIRE(valid.IsValid());
    HFENGINE_REQUIRE(valid == same);
}

HFENGINE_TEST_CASE("unit.core.jobs", "DeterministicQueueRunsJobsInSubmissionOrder")
{
    HFEngine::Core::DeterministicJobQueue queue;
    std::vector<int> values;

    queue.Enqueue("first", [&values]() { values.push_back(1); });
    queue.Enqueue("second", [&values]() { values.push_back(2); });

    HFENGINE_REQUIRE(queue.PendingCount() == 2);
    queue.RunUntilIdle();

    HFENGINE_REQUIRE(queue.PendingCount() == 0);
    HFENGINE_REQUIRE(values.size() == 2);
    HFENGINE_REQUIRE(values[0] == 1);
    HFENGINE_REQUIRE(values[1] == 2);
}
