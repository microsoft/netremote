
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
 *
 * Note that this does not behave in a thread-safe manner. It is unexpected that Enter() and Exit() calls would occur
 * concurrently since it's expected that this is used as stack-allocated objects which will follow typical stack-based
 * lifetime control, which is inherently single-threaded.
 */
struct FunctionTracer
{
    /**
     * @brief Construct a new FunctionTracer object.
     *
     * @param logSeverity The default severity to log with.
     * @param logPrefix The prefix to use for both log messages.
     * @param arguments The arguments the function was called with.
     * @param deferEnter Whether to defer the call to Enter().
     * @param location The source location information of the caller.
     */
    FunctionTracer(plog::Severity logSeverity = LogSeverityEnterDefault, std::string logPrefix = {}, std::vector<std::pair<std::string, std::string>> arguments = {}, bool deferEnter = false, std::source_location location = std::source_location::current());

    /**
     * @brief Destroy the FunctionTracer object.
     */
    virtual ~FunctionTracer();

    /**
     * @brief Print the function entry log message.
     */
    void
    Enter();

    /**
     * @brief Print the function exit log message.
     */
    void
    Exit();

    /**
     * @brief Add an argument to the function tracer. This value will be printed in the entry log message.
     *
     * @param name The name of the argument.
     * @param value The value of the argument.
     */
    void
    AddArgument(std::string name, std::string value);

    /**
     * @brief Add a return value to the function tracer. This value will be printed in the exit log message.
     *
     * @param name The name of the return value.
     * @param value The value of the return value.
     */
    void
    AddReturnValue(std::string name, std::string value);

    /**
     * @brief Mark that the function has succeeded. This will set the log verbosity to info.
     */
    void
    SetSucceeded() noexcept;

    /**
     * @brief Mark that the function has failed. This will set the log verbosity to error.
     */
    void
    SetFailed() noexcept;

    /**
     * @brief Manually set the log severity for the enter log message. This only has an effect if the object was created
     * with deferEnter = true.
     *
     * @param logSeverity The log severity to use when printing the entrance log message.
     */
    void
    SetEnterLogSeverity(plog::Severity logSeverity) noexcept;

    /**
     * @brief Manually set the log severity for the exit log message. This overrides the SetSucceeded and SetFailed methods.
     *
     * @param logSeverityExit The log severity to use when printing the exit log message.
     */
    void
    SetExitLogSeverity(plog::Severity logSeverityExit) noexcept;

protected:
    static constexpr auto LogSeverityEnterDefault{ plog::Severity::debug };
    static constexpr auto LogSeverityExitDefault{ plog::Severity::debug };

private:
    plog::Severity m_logSeverityEnter{ LogSeverityEnterDefault };
    plog::Severity m_logSeverityExit{ LogSeverityExitDefault };
    std::string m_logPrefix;
    std::source_location m_location;
    std::string_view m_functionName;
    std::vector<std::pair<std::string, std::string>> m_arguments;
    std::vector<std::pair<std::string, std::string>> m_returnValues;
    bool m_entered{ false };
    bool m_exited{ false };
};
} // namespace logging

#endif // FUNCTION_TRACER_HXX
