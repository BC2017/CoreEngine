#include "Engine/RHI/CommandList.hpp"

#include <utility>

namespace HFEngine::RHI
{
    ValidationResult ValidateCommandListDesc(const CommandListDesc& desc)
    {
        if (desc.debugName.empty())
        {
            return { false, "Command list debug name must not be empty" };
        }

        return { true, "ok" };
    }

    ValidationResult ValidateRenderPassDesc(const RenderPassDesc& desc)
    {
        if (desc.debugName.empty())
        {
            return { false, "Render pass debug name must not be empty" };
        }

        if (desc.width == 0 || desc.height == 0)
        {
            return { false, "Render pass dimensions must be greater than zero" };
        }

        if (desc.colorAttachments.empty() && !desc.depthAttachment.has_value())
        {
            return { false, "Render pass requires at least one attachment" };
        }

        for (const ColorAttachmentDesc& attachment : desc.colorAttachments)
        {
            if (attachment.debugName.empty())
            {
                return { false, "Color attachment debug name must not be empty" };
            }

            if (!attachment.texture.IsValid())
            {
                return { false, "Color attachment requires a valid texture handle" };
            }

            if (attachment.format == ResourceFormat::Unknown || IsDepthFormat(attachment.format))
            {
                return { false, "Color attachment requires a known color format" };
            }
        }

        if (desc.depthAttachment.has_value())
        {
            const DepthAttachmentDesc& depth = *desc.depthAttachment;
            if (depth.debugName.empty())
            {
                return { false, "Depth attachment debug name must not be empty" };
            }

            if (!depth.texture.IsValid())
            {
                return { false, "Depth attachment requires a valid texture handle" };
            }

            if (!IsDepthFormat(depth.format))
            {
                return { false, "Depth attachment requires a depth format" };
            }
        }

        return { true, "ok" };
    }

    bool CommandListRecorder::Begin(CommandListDesc desc)
    {
        if (!IsValid())
        {
            return false;
        }

        if (state_ != State::Idle)
        {
            return Fail("Command list can only begin from idle state");
        }

        const ValidationResult validation = ValidateCommandListDesc(desc);
        if (!validation.valid)
        {
            return Fail(validation.message);
        }

        state_ = State::Recording;
        commands_.push_back({ RecordedCommandType::BeginCommandList, std::move(desc.debugName) });
        return true;
    }

    bool CommandListRecorder::BeginRenderPass(RenderPassDesc desc)
    {
        if (!IsValid())
        {
            return false;
        }

        if (state_ != State::Recording)
        {
            return Fail("Render pass can only begin while command list is recording");
        }

        const ValidationResult validation = ValidateRenderPassDesc(desc);
        if (!validation.valid)
        {
            return Fail(validation.message);
        }

        state_ = State::InRenderPass;
        graphicsPipelineBound_ = false;
        commands_.push_back({ RecordedCommandType::BeginRenderPass, std::move(desc.debugName) });
        return true;
    }

    bool CommandListRecorder::BindGraphicsPipeline(GraphicsPipelineHandle pipeline)
    {
        if (!IsValid())
        {
            return false;
        }

        if (state_ != State::InRenderPass)
        {
            return Fail("Graphics pipeline can only be bound inside a render pass");
        }

        if (!pipeline.IsValid())
        {
            return Fail("Graphics pipeline bind requires a valid pipeline handle");
        }

        graphicsPipelineBound_ = true;
        commands_.push_back({ RecordedCommandType::BindGraphicsPipeline, "bind graphics pipeline" });
        return true;
    }

    bool CommandListRecorder::DrawIndexed(DrawIndexedDesc desc)
    {
        if (!IsValid())
        {
            return false;
        }

        if (state_ != State::InRenderPass)
        {
            return Fail("Indexed draws require an active render pass");
        }

        if (!graphicsPipelineBound_)
        {
            return Fail("Indexed draws require a bound graphics pipeline");
        }

        const ValidationResult validation = ValidateDrawIndexedDesc(desc);
        if (!validation.valid)
        {
            return Fail(validation.message);
        }

        commands_.push_back({ RecordedCommandType::DrawIndexed, std::move(desc.debugName) });
        return true;
    }

    bool CommandListRecorder::EndRenderPass()
    {
        if (!IsValid())
        {
            return false;
        }

        if (state_ != State::InRenderPass)
        {
            return Fail("Render pass can only end while inside a render pass");
        }

        state_ = State::Recording;
        graphicsPipelineBound_ = false;
        commands_.push_back({ RecordedCommandType::EndRenderPass, "end render pass" });
        return true;
    }

    bool CommandListRecorder::End()
    {
        if (!IsValid())
        {
            return false;
        }

        if (state_ == State::InRenderPass)
        {
            return Fail("Command list cannot end while a render pass is active");
        }

        if (state_ != State::Recording)
        {
            return Fail("Command list can only end while recording");
        }

        state_ = State::Ended;
        commands_.push_back({ RecordedCommandType::EndCommandList, "end command list" });
        return true;
    }

    const ValidationResult& CommandListRecorder::Status() const noexcept
    {
        return status_;
    }

    const std::vector<RecordedCommand>& CommandListRecorder::Commands() const noexcept
    {
        return commands_;
    }

    void CommandListRecorder::Reset() noexcept
    {
        state_ = State::Idle;
        graphicsPipelineBound_ = false;
        status_ = { true, "ok" };
        commands_.clear();
    }

    bool CommandListRecorder::Fail(std::string message)
    {
        status_ = { false, std::move(message) };
        return false;
    }

    bool CommandListRecorder::IsValid() const noexcept
    {
        return status_.valid;
    }
}
