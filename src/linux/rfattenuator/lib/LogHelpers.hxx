#pragma once

#include <format>
#include <iostream>
#include <mutex>
#include <ranges>
#include <source_location>
#include <string>

#include <experimental/scope>

#include "StringHelpers.hxx"

namespace LogHelpers
{
enum class LogLevel : uint8_t {
    None = 0,
    Trace,
    Debug,
    Info,
    Warning,
    Error,
    Critical,
};

namespace detail
{
struct LogState
{
    LogLevel Level{ LogLevel::None };
    uint32_t NumIndents{ 0 };
    std::mutex LogGate{};
};

inline std::string
GetIndentation(uint32_t numIndents)
{
    static constexpr auto Indent = "  ";
    std::string indentation{};

    for ([[maybe_unused]] const auto& i : std::views::iota(0u, numIndents)) {
        indentation += Indent;
    }

    return indentation;
}

inline static LogState&
GetLogStateInstance() noexcept
{
    static LogState logState{
        .Level = LogLevel::None, // Logging disabled by default
        .NumIndents = 0
    };
    return logState;
}
} // namespace detail

/**
 * @brief Set the log level at which messages will be logged. Any log messages
 * below this level will be ignored.
 *
 * @param logLevel The active log level to set.
 */
inline void
SetLogLevel(LogLevel logLevel)
{
    detail::LogState& logState{ detail::GetLogStateInstance() };
    std::unique_lock logLock{ logState.LogGate };
    logState.Level = logLevel;
}

inline auto
TraceFunction(LogLevel logLevel = LogLevel::Debug, std::source_location caller = std::source_location::current())
{
    static constexpr auto LogFormat = "[{}] {}\n";
    static constexpr auto FunctionEnter = '+';
    static constexpr auto FunctionExit = '-';

    detail::LogState& logState{ detail::GetLogStateInstance() };
    const bool loggingEnabled{ logState.Level >= logLevel };
    std::string functionName{ caller.function_name() };

    std::unique_lock logLock{ logState.LogGate };
    std::string indentation{ LogHelpers::detail::GetIndentation(logState.NumIndents) };
    logState.NumIndents++;

    if (loggingEnabled) {
        std::cout << std::format(LogFormat, FunctionEnter, indentation, functionName);
    }

    auto logExit = std::experimental::scope_exit([&, indentation = std::move(indentation), functionName = std::move(functionName)] {
        std::unique_lock logLock{ logState.LogGate };
        if (loggingEnabled) {
            std::cout << std::format(LogFormat, FunctionExit, indentation, functionName);
        }
        logState.NumIndents--;
    });

    return logExit;
}

} // namespace LogHelpers
