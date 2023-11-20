
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
    /**
     * @brief Starts an instance of the specified wpa daemon type.
     * 
     * @param wpaType The type of wpa daemon to start. 
     * @param interfaceName The wlan interface for the daemon to use.
     * @param commandLineArguments The command line arguments to be passed to the daemon binary.
     * @return std::optional<WpaDaemonInstanceHandle> 
     */
    static std::optional<WpaDaemonInstanceHandle> Start(Wpa::WpaType wpaType, const std::string& interfaceName = "wlan0", const std::string& commandLineArguments = "");

    /**
     * @brief Stop a running instance of the specified wpa daemon.
     * 
     * @param instanceHandle The handle to the daemon instance to stop. 
     * @return true 
     * @return false 
     */
    static bool Stop(const WpaDaemonInstanceHandle& instanceHandle);
};

#endif // WPA_DAEMON_MANAGER_HXX