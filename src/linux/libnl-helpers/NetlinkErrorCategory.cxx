
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
    return std::error_code(error, NetlinkErrorCategory());
}

std::error_code
MakeNetlinkErrorCode(int error)
{
    return make_netlink_error_code(error);
}

} // namespace Microsoft::Net::Netlink
