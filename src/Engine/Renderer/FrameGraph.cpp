#include "Engine/Renderer/FrameGraph.hpp"

#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <utility>

namespace HFEngine::Renderer
{
    void FrameGraph::AddPass(FrameGraphPassDesc desc)
    {
        passes_.push_back(std::move(desc));
    }

    const std::vector<FrameGraphPassDesc>& FrameGraph::Passes() const noexcept
    {
        return passes_;
    }

    CompiledFrameGraph FrameGraph::Compile() const
    {
        std::unordered_map<std::string, std::size_t> passNames;
        std::unordered_map<std::string, std::size_t> resourceWriters;

        for (std::size_t index = 0; index < passes_.size(); ++index)
        {
            const FrameGraphPassDesc& pass = passes_[index];

            if (pass.name.empty())
            {
                return { false, "Frame graph pass name must not be empty", {} };
            }

            if (passNames.contains(pass.name))
            {
                return { false, "Duplicate frame graph pass name: " + pass.name, {} };
            }

            passNames[pass.name] = index;

            for (const std::string& resource : pass.writes)
            {
                if (resource.empty())
                {
                    return { false, "Frame graph resource name must not be empty", {} };
                }

                if (resourceWriters.contains(resource))
                {
                    return { false, "Multiple passes write frame graph resource: " + resource, {} };
                }

                resourceWriters[resource] = index;
            }
        }

        std::vector<std::vector<std::size_t>> edges(passes_.size());
        std::vector<std::size_t> incoming(passes_.size(), 0);

        for (std::size_t readerIndex = 0; readerIndex < passes_.size(); ++readerIndex)
        {
            for (const std::string& resource : passes_[readerIndex].reads)
            {
                if (resource.empty())
                {
                    return { false, "Frame graph resource name must not be empty", {} };
                }

                const auto writer = resourceWriters.find(resource);
                if (writer == resourceWriters.end() || writer->second == readerIndex)
                {
                    continue;
                }

                edges[writer->second].push_back(readerIndex);
                ++incoming[readerIndex];
            }
        }

        std::queue<std::size_t> ready;
        for (std::size_t index = 0; index < incoming.size(); ++index)
        {
            if (incoming[index] == 0)
            {
                ready.push(index);
            }
        }

        CompiledFrameGraph compiled;
        compiled.valid = true;
        compiled.message = "ok";

        while (!ready.empty())
        {
            const std::size_t passIndex = ready.front();
            ready.pop();
            compiled.executionOrder.push_back(passes_[passIndex].name);

            for (const std::size_t dependent : edges[passIndex])
            {
                --incoming[dependent];
                if (incoming[dependent] == 0)
                {
                    ready.push(dependent);
                }
            }
        }

        if (compiled.executionOrder.size() != passes_.size())
        {
            return { false, "Frame graph contains a dependency cycle", {} };
        }

        return compiled;
    }

    void FrameGraph::Clear() noexcept
    {
        passes_.clear();
    }
}
