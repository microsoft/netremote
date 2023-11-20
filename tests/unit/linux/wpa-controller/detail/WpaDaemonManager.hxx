
#ifndef WPA_DAEMON_MANAGER_HXX
#define WPA_DAEMON_MANAGER_HXX

#include <filesystem>
#include <optional>
#include <string>

#include <unistd.h>
#include <sys/types.h>

#include "detail/WpaDaemonManager.hxx"
#include <Wpa/WpaCore.hxx>

struct WpaDaemonInstanceHandle
{
    Wpa::WpaType WpaType;
    pid_t Pid;
};

struct WpaDaemonManager
{
    /**
     * @brief The default interface name used, if not provided.
     */
    static constexpr auto InterfaceNameDefault{"wlan0"};

    /**
     * @brief The default path to the daemon control socket, if not specified.
     */
    static constexpr auto ControlSocketPathBase{"/run/"};

    /**
     * @brief Create a default configuration file for the specified wpa daemon type.
     * 
     * @param wpaType The type of wpa daemon to create the configuration file for.
     * @param interfaceName The wlan interface for the daemon to use.
     * @return std::filesystem::path The path to the created configuration file.
     */
    static std::filesystem::path CreateDefaultConfigurationFile(Wpa::WpaType wpaType, const std::string& interfaceName);

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