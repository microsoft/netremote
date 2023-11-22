
#ifndef NOTSTD_EXCEPTIONS_HXX
#define NOTSTD_EXCEPTIONS_HXX

#include <format>
#include <source_location>
#include <stdexcept>

namespace notstd
{
/**
 * @brief Simple exception to indicate a function is not implemented.
 */
struct NotImplementedException :
    public std::logic_error
{
    NotImplementedException(std::source_location location = std::source_location::current(), std::string_view message = "") :
        std::logic_error(std::format("{}({}:{}) `{}` not implemented {}", location.file_name(), location.line(), location.column(), location.function_name(), (!std::empty(message) ? std::string(": ").append(message) : "")))
    {
    }
};
} // namespace notstd

#endif // NOTSTD_EXCEPTIONS_HXX
