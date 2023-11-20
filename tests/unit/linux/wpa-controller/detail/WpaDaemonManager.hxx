
#ifndef WPA_DAEMON_MANAGER_HXX
#define WPA_DAEMON_MANAGER_HXX

#include <optional>
#include <string>

#include <unistd.h>
#include <sys/types.h>

#include <Wpa/WpaCore.hxx>

struct WpaDaemonInstanceHandle
{
    Wpa::WpaType WpaType;
    pid_t Pid;
};

struct WpaDaemonManager
{
    static std::optional<WpaDaemonInstanceHandle> Start(Wpa::WpaType wpaType, const std::string& interfaceName = "wlan0", const std::string& commandLineArguments = "");

    static bool Stop(const WpaDaemonInstanceHandle& instanceToken);
};

#endif // WPA_DAEMON_MANAGER_HXX