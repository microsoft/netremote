
#ifndef NETLINK_EXCEPTION_HXX
#define NETLINK_EXCEPTION_HXX

#include <string>
#include <system_error>

namespace Microsoft::Net::Netlink
{
/**
 * @brief std compatible exception for Netlink errors.
 */
struct NetlinkException :
    public std::system_error
{
public:
    /**
     * @brief Construct a new NetlinkException object.
     *
     * @param error The netlink error code. This must be one of the NLE_* values from netlink/errno.h.
     * @param what The error message.
     */
    explicit NetlinkException(int error, const char* what = "");

    /**
     * @brief Construct a new NetlinkException object. Overload accepting a std::string.
     *
     * @param error The netlink error code. This must be one of the NLE_* values from netlink/errno.h.
     * @param what The error message.
     */
    NetlinkException(int error, const std::string& what);

    /**
     * @brief Create a NetlinkException and log it.
     * 
     * @param error The netlink error code. This must be one of the NLE_* values from netlink/errno.h. 
     * @param what The error message.
     * @return NetlinkException 
     */
    static NetlinkException
    CreateLogged(int error, const char* what = "");

    /**
     * @brief Create a NetlinkException and log it. Overload accepting a std::string.
     * 
     * @param error The netlink error code. This must be one of the NLE_* values from netlink/errno.h.
     * @param what The error message.
     * @return NetlinkException 
     */
    static NetlinkException
    CreateLogged(int error, const std::string& what);
};

/**
 * @brief std-style type alias for NetlinkException.
 */
using netlink_exception = NetlinkException;

} // namespace Microsoft::Net::Netlink

#endif // NETLINK_EXCEPTION_HXX
