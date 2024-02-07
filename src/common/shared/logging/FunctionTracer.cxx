
#include <format>
#include <sstream>

#include <logging/FunctionTracer.hxx>
#include <plog/Log.h>

using namespace logging;

namespace detail
{
std::string
BuildValueList(const std::vector<std::pair<std::string, std::string>>& values, std::string_view prefix, std::string_view keyValueSeparator = "=", std::string_view delimeter = ", ")
{
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
        valueListBuilder << name << keyValueSeparator << value << "'";
    }

    return valueListBuilder.str();
}
} // namespace detail

FunctionTracer::FunctionTracer(std::string logPrefix, std::vector<std::pair<std::string, std::string>> arguments, bool deferEnter, std::source_location location) :
    m_logPrefix(std::move(logPrefix)),
    m_location(std::move(location)),
    m_functionName(m_location.function_name()),
    m_arguments(std::move(arguments))
{
    // Attempt to parse the function name, removing the return type, namespace prefixes, and arguments.
    const auto openingBracketPos = m_functionName.find_first_of('(');
    if (openingBracketPos != m_functionName.npos) {
        m_functionName.remove_suffix(m_functionName.size() - openingBracketPos);
    }

    const auto lastColonPos = m_functionName.find_last_of(':');
    if (lastColonPos != m_functionName.npos) {
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
    if (m_entered) {
        return;
    }

    m_entered = true;
    auto arguments = detail::BuildValueList(m_arguments, " with arguments ");
    LOGI << std::format("{} +{}{}", m_logPrefix, m_functionName, arguments);
}

void
FunctionTracer::Exit()
{
    if (m_exited) {
        return;
    }

    auto returnValues = detail::BuildValueList(m_returnValues, " returning ");
    PLOG(m_exitLogSeverity) << std::format("{} -{}{}", m_logPrefix, m_functionName, returnValues);
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
    m_exitLogSeverity = plog::Severity::info;
}

void
FunctionTracer::SetFailed() noexcept
{
    m_exitLogSeverity = plog::Severity::error;
}

void
FunctionTracer::SetExitLogSeverity(plog::Severity severity) noexcept
{
    m_exitLogSeverity = severity;
}
