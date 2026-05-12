#include "Engine/Core/JobSystem.hpp"

#include <utility>

namespace HFEngine::Core
{
    void DeterministicJobQueue::Enqueue(std::string name, std::function<void()> execute)
    {
        jobs_.push_back({ std::move(name), std::move(execute) });
    }

    std::size_t DeterministicJobQueue::PendingCount() const noexcept
    {
        return jobs_.size();
    }

    void DeterministicJobQueue::RunUntilIdle()
    {
        while (!jobs_.empty())
        {
            Job job = std::move(jobs_.front());
            jobs_.erase(jobs_.begin());
            job.execute();
        }
    }

    void DeterministicJobQueue::Clear() noexcept
    {
        jobs_.clear();
    }
}
