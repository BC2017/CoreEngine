#pragma once

#include <string>
#include <string_view>
#include <vector>

namespace HFEngine::Core
{
    enum class LogLevel
    {
        Trace,
        Info,
        Warning,
        Error,
        Fatal
    };

    struct LogEntry
    {
        LogLevel level = LogLevel::Info;
        std::string category;
        std::string message;
    };

    class MemoryLogSink
    {
    public:
        void Write(LogEntry entry);
        [[nodiscard]] const std::vector<LogEntry>& Entries() const noexcept;
        void Clear() noexcept;

    private:
        std::vector<LogEntry> entries_;
    };

    [[nodiscard]] const char* ToString(LogLevel level) noexcept;
}
