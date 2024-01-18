
#ifndef NET_REMOTE_CLI_DATA_HXX
#define NET_REMOTE_CLI_DATA_HXX

#include <string>

#include <microsoft/net/remote/NetRemoteClient.hxx>
#include <microsoft/net/remote/protocol/NetRemoteProtocol.hxx>

namespace Microsoft::Net::Remote
{
struct NetRemoteCli;

/**
 * @brief Data associated with the NetRemote CLI. This is used to store command-line arguments and other data associated
 * with CLI configuration and operation.
 */
struct NetRemoteCliData
{
    std::string ServerAddress{ Protocol::NetRemoteProtocol::AddressDefault };
    NetRemoteCommandId Command{ NetRemoteCommandId::None };
};
} // namespace Microsoft::Net::Remote

#endif // NET_REMOTE_CLI_DATA_HXX
