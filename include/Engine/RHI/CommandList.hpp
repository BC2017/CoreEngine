#pragma once

#include "Engine/RHI/Pipeline.hpp"
#include "Engine/RHI/Resource.hpp"

#include <array>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace HFEngine::RHI
{
    enum class CommandQueueType
    {
        Graphics,
        Compute,
        Copy
    };

    enum class RecordedCommandType
    {
        BeginCommandList,
        BeginRenderPass,
        BindGraphicsPipeline,
        DrawIndexed,
        EndRenderPass,
        EndCommandList
    };

    struct CommandListDesc
    {
        std::string debugName;
        CommandQueueType queueType = CommandQueueType::Graphics;
    };

    struct ColorAttachmentDesc
    {
        std::string debugName;
        TextureHandle texture;
        ResourceFormat format = ResourceFormat::Bgra8Unorm;
        bool clear = true;
        std::array<float, 4> clearColor{ 0.0f, 0.0f, 0.0f, 1.0f };
    };

    struct DepthAttachmentDesc
    {
        std::string debugName;
        TextureHandle texture;
        ResourceFormat format = ResourceFormat::D32Float;
        bool clear = true;
        float clearDepth = 1.0f;
    };

    struct RenderPassDesc
    {
        std::string debugName;
        std::uint32_t width = 0;
        std::uint32_t height = 0;
        std::vector<ColorAttachmentDesc> colorAttachments;
        std::optional<DepthAttachmentDesc> depthAttachment;
    };

    struct RecordedCommand
    {
        RecordedCommandType type = RecordedCommandType::BeginCommandList;
        std::string debugName;
    };

    [[nodiscard]] ValidationResult ValidateCommandListDesc(const CommandListDesc& desc);
    [[nodiscard]] ValidationResult ValidateRenderPassDesc(const RenderPassDesc& desc);

    class CommandListRecorder
    {
    public:
        [[nodiscard]] bool Begin(CommandListDesc desc);
        [[nodiscard]] bool BeginRenderPass(RenderPassDesc desc);
        [[nodiscard]] bool BindGraphicsPipeline(GraphicsPipelineHandle pipeline);
        [[nodiscard]] bool DrawIndexed(DrawIndexedDesc desc);
        [[nodiscard]] bool EndRenderPass();
        [[nodiscard]] bool End();

        [[nodiscard]] const ValidationResult& Status() const noexcept;
        [[nodiscard]] const std::vector<RecordedCommand>& Commands() const noexcept;
        void Reset() noexcept;

    private:
        enum class State
        {
            Idle,
            Recording,
            InRenderPass,
            Ended
        };

        [[nodiscard]] bool Fail(std::string message);
        [[nodiscard]] bool IsValid() const noexcept;

        State state_ = State::Idle;
        bool graphicsPipelineBound_ = false;
        ValidationResult status_{ true, "ok" };
        std::vector<RecordedCommand> commands_;
    };
}
