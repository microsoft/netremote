
#ifndef WPA_DAEMON_MANAGER_HXX
#define WPA_DAEMON_MANAGER_HXX

#include <filesystem>
#include <optional>
#include <string_view>

#include <unistd.h>
#include <sys/types.h>

#include "detail/WpaDaemonManager.hxx"
#include <Wpa/WpaCore.hxx>

/**
 * @brief Handle referencing an running wpa daemon instance.
 */
struct WpaDaemonInstanceHandle
{
    Wpa::WpaType WpaType;
    pid_t Pid;
};

/**
 * @brief Manages instaces of wpa daemons, including creating and destroying
 * them.
 */
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
     * @brief Create and write a default configuration file to disk for the
     * specified wpa daemon type. The configuration file will be written to the
     * system tmeporary directory, and so will not persist across reboots.
     * 
     * @param wpaType The type of wpa daemon to create the configuration file for.
     * @param interfaceName The wlan interface for the daemon to use.
     * @return std::filesystem::path The path to the created configuration file.
     */
    static std::filesystem::path CreateAndWriteDefaultConfigurationFile(Wpa::WpaType wpaType, std::string_view interfaceName = InterfaceNameDefault);

    /**
     * @brief Starts an instance of the specified daemon type using a default
     * configuration file.
     * 
     * @param wpaType The type of wpa daemon to start. 
     * @param interfaceName The optional wlan interface for the daemon to use.
     * @return std::optional<WpaDaemonInstanceHandle> 
     */
    static std::optional<WpaDaemonInstanceHandle> StartDefault(Wpa::WpaType wpaType, std::string_view interfaceName = InterfaceNameDefault);

    /**
     * @brief Starts an instance of the specified wpa daemon type.
     * 
     * @param wpaType The type of wpa daemon to start. 
     * @param interfaceName The wlan interface for the daemon to use.
     * @param configurationFilePath The path to the daemon configuration file.
     * @param extraCommandLineArguments The command line arguments to be passed to the daemon binary.
     * @return std::optional<WpaDaemonInstanceHandle> 
     */
    static std::optional<WpaDaemonInstanceHandle> Start(Wpa::WpaType wpaType, std::string_view interfaceName, const std::filesystem::path& configurationFilePath, std::string_view extraCommandLineArguments = "");

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