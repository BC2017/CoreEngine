#include "TestHarness.hpp"

#include <algorithm>
#include <cstring>
#include <iostream>
#include <ostream>
#include <sstream>
#include <utility>

namespace HFEngine::Testing
{
    Registry& Registry::Instance()
    {
        static Registry registry;
        return registry;
    }

    void Registry::Add(TestCase testCase)
    {
        tests_.push_back(std::move(testCase));
    }

    const std::vector<TestCase>& Registry::Tests() const noexcept
    {
        return tests_;
    }

    Registrar::Registrar(std::string_view suite, std::string_view name, std::function<void()> body)
    {
        Registry::Instance().Add({ suite, name, std::move(body) });
    }

    AssertionFailure::AssertionFailure(std::string message, std::source_location location)
        : message_(std::move(message))
        , location_(location)
    {
    }

    const char* AssertionFailure::what() const noexcept
    {
        return message_.c_str();
    }

    const std::source_location& AssertionFailure::Location() const noexcept
    {
        return location_;
    }

    SkipTest::SkipTest(std::string message)
        : message_(std::move(message))
    {
    }

    const char* SkipTest::what() const noexcept
    {
        return message_.c_str();
    }

    void Require(bool condition, std::string_view expression, std::source_location location)
    {
        if (!condition)
        {
            std::ostringstream message;
            message << "Requirement failed: " << expression;
            throw AssertionFailure(message.str(), location);
        }
    }

    void Skip(std::string message)
    {
        throw SkipTest(std::move(message));
    }

    namespace
    {
        struct Options
        {
            bool listOnly = false;
            std::string filter;
        };

        Options ParseOptions(int argc, char** argv)
        {
            Options options;

            for (int index = 1; index < argc; ++index)
            {
                if (std::strcmp(argv[index], "--list") == 0)
                {
                    options.listOnly = true;
                    continue;
                }

                if (std::strcmp(argv[index], "--filter") == 0 && index + 1 < argc)
                {
                    options.filter = argv[++index];
                    continue;
                }
            }

            return options;
        }

        bool MatchesFilter(const TestCase& testCase, const std::string& filter)
        {
            if (filter.empty())
            {
                return true;
            }

            const std::string fullName = std::string(testCase.suite) + "." + std::string(testCase.name);
            return fullName.find(filter) != std::string::npos;
        }
    }

    int RunAllTests(int argc, char** argv, std::ostream& output)
    {
        const Options options = ParseOptions(argc, argv);
        const std::vector<TestCase>& tests = Registry::Instance().Tests();

        if (options.listOnly)
        {
            for (const TestCase& testCase : tests)
            {
                if (MatchesFilter(testCase, options.filter))
                {
                    output << testCase.suite << "." << testCase.name << '\n';
                }
            }

            return 0;
        }

        int passed = 0;
        int failed = 0;
        int skipped = 0;

        for (const TestCase& testCase : tests)
        {
            if (!MatchesFilter(testCase, options.filter))
            {
                continue;
            }

            output << "[ RUN      ] " << testCase.suite << "." << testCase.name << '\n';

            try
            {
                testCase.body();
                ++passed;
                output << "[       OK ] " << testCase.suite << "." << testCase.name << '\n';
            }
            catch (const SkipTest& skip)
            {
                ++skipped;
                output << "[  SKIPPED ] " << testCase.suite << "." << testCase.name << '\n';
                output << "             " << skip.what() << '\n';
            }
            catch (const AssertionFailure& failure)
            {
                ++failed;
                output << "[  FAILED  ] " << testCase.suite << "." << testCase.name << '\n';
                output << "             " << failure.what() << '\n';
                output << "             " << failure.Location().file_name()
                       << ':' << failure.Location().line() << '\n';
            }
            catch (const std::exception& exception)
            {
                ++failed;
                output << "[  FAILED  ] " << testCase.suite << "." << testCase.name << '\n';
                output << "             Unexpected exception: " << exception.what() << '\n';
            }
            catch (...)
            {
                ++failed;
                output << "[  FAILED  ] " << testCase.suite << "." << testCase.name << '\n';
                output << "             Unknown non-standard exception\n";
            }
        }

        output << "[==========] " << passed + failed + skipped << " test(s) ran\n";
        output << "[  PASSED  ] " << passed << " test(s)\n";
        output << "[  SKIPPED ] " << skipped << " test(s)\n";

        if (failed > 0)
        {
            output << "[  FAILED  ] " << failed << " test(s)\n";
            return 1;
        }

        return 0;
    }
}
