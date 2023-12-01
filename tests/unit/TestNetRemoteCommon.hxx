
#ifndef TEST_NET_REMOTE_COMMON_HXX
#define TEST_NET_REMOTE_COMMON_HXX

#include <chrono>
#include <string_view>

namespace Microsoft::Net::Remote::Test
{
using namespace std::chrono_literals;

constexpr auto RemoteServiceAddressHttp = "localhost:5047";
constexpr auto RemoteServiceConnectionTimeout = 3s;
} // namespace Micosoft::Net::Remote::Test

#endif // TEST_NET_REMOTE_COMMON_HXX
