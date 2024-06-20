
#include <stdexcept>
#include <string>
#include <system_error>

#include <microsoft/net/netlink/NetlinkErrorCategory.hxx>
#include <netlink/errno.h>

namespace Microsoft::Net::Netlink
{

const char*
NetlinkErrorCategory::name() const noexcept
{
    static constexpr const char* Name = "Netlink";
    return Name;
}

std::string
NetlinkErrorCategory::message(int error) const
{
    return nl_geterror(error);
}

std::error_condition
NetlinkErrorCategory::default_error_condition(int error) const noexcept
{
    return std::error_condition(error, *this);
}

std::error_code
make_netlink_error_code(int error)
{
    return std::error_code(error, GetNetlinkErrorCategory());
}

std::error_code
MakeNetlinkErrorCode(int error)
{
    if (error < 0) {
        throw std::runtime_error("Netlink error codes must be non-negative; this is a programming error");
    }

    return make_netlink_error_code(error);
}

const NetlinkErrorCategory&
GetNetlinkErrorCategory()
{
    static NetlinkErrorCategory instance;
    return instance;
}

} // namespace Microsoft::Net::Netlink
