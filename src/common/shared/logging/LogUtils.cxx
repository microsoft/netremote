
#include <chrono>
#include <cstdint>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>
#include <string_view>

#include <logging/LogUtils.hxx>
#include <plog/Severity.h>

std::string
logging::GetLogName(std::string_view componentName)
{
    const auto now = std::chrono::system_clock::now();
    const std::time_t t_c = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;

    ss << std::put_time(std::localtime(&t_c), "%Y%m%d") // NOLINT(concurrency-mt-unsafe)
       << "-LogNetRemote-"
       << componentName
       << ".txt";

    return ss.str();
}

plog::Severity
logging::LogVerbosityToPlogSeverity(uint32_t verbosity) noexcept
{
    switch (verbosity) {
    case 0:
        return plog::warning;
    case 1:
        return plog::info;
    case 2:
        return plog::debug;
    case 3:
    default:
        return plog::verbose;
    }
}
