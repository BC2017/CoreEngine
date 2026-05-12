#pragma once

#include <string>
#include <string_view>
#include <vector>

namespace HFEngine::Renderer
{
    struct FrameGraphPassDesc
    {
        std::string name;
        std::vector<std::string> reads;
        std::vector<std::string> writes;
    };

    struct CompiledFrameGraph
    {
        bool valid = false;
        std::string message;
        std::vector<std::string> executionOrder;
    };

    class FrameGraph
    {
    public:
        void AddPass(FrameGraphPassDesc desc);
        [[nodiscard]] const std::vector<FrameGraphPassDesc>& Passes() const noexcept;
        [[nodiscard]] CompiledFrameGraph Compile() const;
        void Clear() noexcept;

    private:
        std::vector<FrameGraphPassDesc> passes_;
    };
}
