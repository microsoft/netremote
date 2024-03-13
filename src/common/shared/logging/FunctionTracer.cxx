
#include <cstddef>
#include <format>
#include <source_location>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <logging/FunctionTracer.hxx>
#include <plog/Log.h>
#include <plog/Severity.h>

using namespace logging;

namespace detail
{
std::string
BuildValueList(const std::vector<std::pair<std::string, std::string>>& values, std::string_view prefix, std::string_view keyValueSeparator = "=", std::string_view delimeter = ", ")
{
    static constexpr auto ValueWrapperCharacter = '\'';

    if (std::empty(values)) {
        return {};
    }

    std::ostringstream valueListBuilder;
    valueListBuilder << prefix;
    for (std::size_t i = 0; i < std::size(values); i++) {
        const auto& [name, value] = values[i];
        if (i > 0) {
            valueListBuilder << delimeter;
        }
        valueListBuilder << name << keyValueSeparator << ValueWrapperCharacter << value << ValueWrapperCharacter;
    }

    return valueListBuilder.str();
}
} // namespace detail

FunctionTracer::FunctionTracer(plog::Severity logSeverity, std::string logPrefix, std::vector<std::pair<std::string, std::string>> arguments, bool deferEnter, std::source_location location) :
    m_logSeverityEnter(logSeverity),
    m_logSeverityExit(logSeverity),
    m_logPrefix(std::move(logPrefix)),
    m_location(location),
    m_functionName(m_location.function_name()),
    m_arguments(std::move(arguments))
{
    // Attempt to parse the function name, removing the return type, namespace prefixes, and arguments.
    const auto openingBracketPos = m_functionName.find_first_of('(');
    if (openingBracketPos != std::string_view::npos) {
        m_functionName.remove_suffix(m_functionName.size() - openingBracketPos);
    }

    const auto lastColonPos = m_functionName.find_last_of(':');
    if (lastColonPos != std::string_view::npos) {
        m_functionName.remove_prefix(lastColonPos + 1);
    }

    if (!deferEnter) {
        Enter();
    }
}

FunctionTracer::~FunctionTracer()
{
    Exit();
}

void
FunctionTracer::Enter()
{
    // Note: this is not thread-safe.
    if (m_entered) {
        return;
    }

    m_entered = true;
    const auto arguments = detail::BuildValueList(m_arguments, " with arguments ");
    PLOG(m_logSeverityEnter) << std::format("{} +{}{}", m_logPrefix, m_functionName, arguments);
}

void
FunctionTracer::Exit()
{
    // Note: this is not thread-safe.
    if (m_exited) {
        return;
    }

    auto returnValues = detail::BuildValueList(m_returnValues, " returning ");
    PLOG(m_logSeverityExit) << std::format("{} -{}{}", m_logPrefix, m_functionName, returnValues);
    m_exited = true;
}

void
FunctionTracer::AddArgument(std::string name, std::string value)
{
    m_arguments.emplace_back(std::move(name), std::move(value));
}

void
FunctionTracer::AddReturnValue(std::string name, std::string value)
{
    m_returnValues.emplace_back(std::move(name), std::move(value));
}

void
FunctionTracer::SetSucceeded() noexcept
{
    m_logSeverityExit = plog::Severity::info;
}

void
FunctionTracer::SetFailed() noexcept
{
    m_logSeverityExit = plog::Severity::error;
}

void
FunctionTracer::SetEnterLogSeverity(plog::Severity logSeverity) noexcept
{
    if (m_entered) {
        LOGW << "warning: calling SetEnterLogSeverity after entered log has already been printed; this will have no effect.";
    }

    m_logSeverityEnter = logSeverity;
}

void
FunctionTracer::SetExitLogSeverity(plog::Severity logSeverityExit) noexcept
{
    if (m_exited) {
        LOGW << "warning: calling SetExitLogSeverity after exited log has already been printed; this will have no effect.";
    }

    m_logSeverityExit = logSeverityExit;
}
