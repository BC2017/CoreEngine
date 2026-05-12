#pragma once

#include <functional>
#include <string>
#include <string_view>
#include <vector>

namespace HFEngine::Core
{
    struct Job
    {
        std::string name;
        std::function<void()> execute;
    };

    class DeterministicJobQueue
    {
    public:
        void Enqueue(std::string name, std::function<void()> execute);
        [[nodiscard]] std::size_t PendingCount() const noexcept;
        void RunUntilIdle();
        void Clear() noexcept;

    private:
        std::vector<Job> jobs_;
    };
}
