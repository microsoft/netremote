
#ifndef FUNCTION_TRACER_HXX
#define FUNCTION_TRACER_HXX

#include <source_location>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <plog/Severity.h>

namespace logging
{
/**
 * @brief Traces a function's entry and exit, logging the arguments and return values.
 */
struct FunctionTracer
{
    FunctionTracer(std::string logPrefix = {}, std::vector<std::pair<std::string, std::string>> arguments = {}, bool deferEnter = false, std::source_location location = std::source_location::current());

    virtual ~FunctionTracer();

    void
    Enter();

    void
    Exit();

    void
    AddArgument(std::string name, std::string value);

    void
    AddReturnValue(std::string name, std::string value);

    void
    SetSucceeded() noexcept;

    void
    SetFailed() noexcept;

    void
    SetExitLogSeverity(plog::Severity severity) noexcept;

private:
    std::string m_logPrefix;
    std::source_location m_location;
    std::string_view m_functionName;
    std::vector<std::pair<std::string, std::string>> m_arguments;
    std::vector<std::pair<std::string, std::string>> m_returnValues;
    bool m_entered{ false };
    bool m_exited{ false };
    plog::Severity m_exitLogSeverity{ plog::Severity::info };
};
} // namespace logging

#endif // FUNCTION_TRACER_HXX
