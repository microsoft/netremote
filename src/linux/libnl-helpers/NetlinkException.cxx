
#include <format>
#include <string>
#include <system_error>

#include <microsoft/net/netlink/NetlinkErrorCategory.hxx>
#include <microsoft/net/netlink/NetlinkException.hxx>
#include <plog/Log.h>

using namespace Microsoft::Net::Netlink;

NetlinkException::NetlinkException(int error, const char *what) :
    std::system_error(MakeNetlinkErrorCode(error), what)
{}

NetlinkException::NetlinkException(int error, const std::string &what) :
    NetlinkException(error, what.c_str())
{}

/* static */
NetlinkException
NetlinkException::CreateLogged(int error, const char *what)
{
    NetlinkException netlinkException(error, what);
    LOGE << std::format("Netlink error ({}): {} ({})", netlinkException.code().value(), netlinkException.what(), netlinkException.code().message());
    return netlinkException;
}

/* static */
NetlinkException
NetlinkException::CreateLogged(int error, const std::string &what)
{
    return CreateLogged(error, what.c_str());
}
