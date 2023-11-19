
#ifndef WPA_DAEMON_MANAGER_HXX
#define WPA_DAEMON_MANAGER_HXX

#include <optional>

#include <Wpa/WpaCore.hxx>

struct WpaDaemonInstanceToken
{
    Wpa::WpaType WpaType;
};

struct WpaDaemonManager
{
    static std::optional<WpaDaemonInstanceToken> Start(Wpa::WpaType wpaType);

    static bool Stop(const WpaDaemonInstanceToken& instanceToken);
};

#endif // WPA_DAEMON_MANAGER_HXX