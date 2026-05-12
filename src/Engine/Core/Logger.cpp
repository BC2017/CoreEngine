#include "Engine/Core/Logger.hpp"

#include <utility>

namespace HFEngine::Core
{
    void MemoryLogSink::Write(LogEntry entry)
    {
        entries_.push_back(std::move(entry));
    }

    const std::vector<LogEntry>& MemoryLogSink::Entries() const noexcept
    {
        return entries_;
    }

    void MemoryLogSink::Clear() noexcept
    {
        entries_.clear();
    }

    const char* ToString(LogLevel level) noexcept
    {
        switch (level)
        {
        case LogLevel::Trace:
            return "Trace";
        case LogLevel::Info:
            return "Info";
        case LogLevel::Warning:
            return "Warning";
        case LogLevel::Error:
            return "Error";
        case LogLevel::Fatal:
            return "Fatal";
        }

        return "Unknown";
    }
}
