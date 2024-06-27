
#ifndef NETLINK_ERROR_CATEGORY_HXX
#define NETLINK_ERROR_CATEGORY_HXX

#include <string>
#include <system_error>

namespace Microsoft::Net::Netlink
{
/**
 * @brief Error category for errors originating from the netlink library.
 */
struct NetlinkErrorCategory :
    public std::error_category
{
    /**
     * @brief The name of this category.
     *
     * @return const char* "Netlink".
     */
    const char*
    name() const noexcept override;

    /**
     * @brief A string representation of the error code.
     *
     * @param error The error code to convert to a string.
     * @return std::string
     */
    std::string
    message(int error) const override;

    /**
     * @brief The default error condition for the given error code.
     *
     * @param error The error code to convert to an error condition.
     * @return std::error_condition
     */
    std::error_condition
    default_error_condition(int error) const noexcept override;
};

/**
 * @brief Constructs a std::error_code from a netlink error code.
 *
 * This function uses the std naming convention.
 *
 * @param error The netlink error code. Must be positive.
 * @return std::error_code
 */
[[nodiscard]] std::error_code
make_netlink_error_code(int error);

/**
 * @brief Alias for the make_netlink_error_code function.
 *
 * This function uses the project naming convention.
 *
 * @param error
 * @return std::error_code
 */
[[nodiscard]] std::error_code
MakeNetlinkErrorCode(int error);

/**
 * @brief Get the NetlinkErrorCategory object instance.
 * 
 * @return const NetlinkErrorCategory& 
 */
[[nodiscard]] const NetlinkErrorCategory&
GetNetlinkErrorCategory();

} // namespace Microsoft::Net::Netlink

#endif // NETLINK_ERROR_CATEGORY_HXX
