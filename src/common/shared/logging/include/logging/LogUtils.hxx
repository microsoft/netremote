
#ifndef LOG_UTILS_HXX
#define LOG_UTILS_HXX

#include <cstdint>
#include <string_view>
#include <string>

#include <plog/Severity.h>

namespace logging
{
/**
 * @brief Returns the name of the log file to be used.
 * Format is <date>-LogNetRemote-<componentName>.txt
 * For example, if the date was Jan 2, 2023 and the componentName was 'server',
 * then the name would be '20230102-LogNetRemote-server.txt'.
 *
 * @param componentName The name of the component the log is for. Eg. 'server', 'client', etc.
 * @return std::string
 */
std::string GetLogName(std::string_view componentName);

/**
 * @brief Converts a log verbosity level to a plog severity level.
 * 
 * @param verbosity The log verbosity level.
 * @return plog::Severity
 */
plog::Severity LogVerbosityToPlogSeverity(uint8_t verbosity) noexcept;

} // namespace logging

#endif // LOG_UTILS_HXX
