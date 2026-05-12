#pragma once

#include <exception>
#include <functional>
#include <iosfwd>
#include <source_location>
#include <string>
#include <string_view>
#include <vector>

namespace HFEngine::Testing
{
    struct TestCase
    {
        std::string_view suite;
        std::string_view name;
        std::function<void()> body;
    };

    class Registry
    {
    public:
        static Registry& Instance();

        void Add(TestCase testCase);
        [[nodiscard]] const std::vector<TestCase>& Tests() const noexcept;

    private:
        std::vector<TestCase> tests_;
    };

    class Registrar
    {
    public:
        Registrar(std::string_view suite, std::string_view name, std::function<void()> body);
    };

    class AssertionFailure final : public std::exception
    {
    public:
        AssertionFailure(std::string message, std::source_location location);

        [[nodiscard]] const char* what() const noexcept override;
        [[nodiscard]] const std::source_location& Location() const noexcept;

    private:
        std::string message_;
        std::source_location location_;
    };

    class SkipTest final : public std::exception
    {
    public:
        explicit SkipTest(std::string message);

        [[nodiscard]] const char* what() const noexcept override;

    private:
        std::string message_;
    };

    void Require(
        bool condition,
        std::string_view expression,
        std::source_location location = std::source_location::current());

    [[noreturn]] void Skip(std::string message);

    int RunAllTests(int argc, char** argv, std::ostream& output);
}

#define HFENGINE_CONCAT_IMPL(left, right) left##right
#define HFENGINE_CONCAT(left, right) HFENGINE_CONCAT_IMPL(left, right)

#define HFENGINE_TEST_CASE(suite, name) \
    static void HFENGINE_CONCAT(HFEngineTest_, __LINE__)(); \
    static ::HFEngine::Testing::Registrar HFENGINE_CONCAT(HFEngineRegistrar_, __LINE__)(suite, name, HFENGINE_CONCAT(HFEngineTest_, __LINE__)); \
    static void HFENGINE_CONCAT(HFEngineTest_, __LINE__)()

#define HFENGINE_REQUIRE(expression) \
    ::HFEngine::Testing::Require(static_cast<bool>(expression), #expression)

#define HFENGINE_SKIP(message) \
    ::HFEngine::Testing::Skip(message)
