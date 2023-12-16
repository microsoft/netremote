
#ifndef WPA_DAEMON_INSTANCE_HXX
#define WPA_DAEMON_INSTANCE_HXX

#include <format>
#include <iostream>

#include "WpaDaemonManager.hxx"
#include <Wpa/WpaCore.hxx>

/**
 * @brief Represents a running instance of a WPA daemon.
 */
struct IWpaDaemonInstance
{
    virtual ~IWpaDaemonInstance() = default;

    virtual bool
    Start() = 0;

    virtual bool
    Stop() = 0;
};

/**
 * @brief Concrete implementation of IWpaDaemonInstance for a specific WPA daemon type.
 *
 * @tparam wpaType The type of wpa instance to manage.
 */
template <Wpa::WpaType wpaType>
struct WpaDaemonInstance :
    public IWpaDaemonInstance
{
    WpaDaemonInstance() :
        m_wpaDaemonName(Wpa::GetWpaTypeDaemonBinaryName(m_wpaType))
    {
    }

    bool
    Start() override
    {
        auto instanceToken = WpaDaemonManager::StartDefault(m_wpaType);
        if (!instanceToken.has_value()) {
            std::cerr << std::format("Failed to start {} daemon instance\n", m_wpaDaemonName);
            return false;
        }

        m_instanceToken = std::move(instanceToken);
        return true;
    }

    bool
    Stop() override
    {
        if (!m_instanceToken.has_value()) {
            std::cout << std::format("Warning: No {} daemon instance running\n", m_wpaDaemonName);
            return true;
        }

        if (!WpaDaemonManager::Stop(m_instanceToken.value())) {
            std::cerr << std::format("Failed to stop {} daemon instance\n", m_wpaDaemonName);
            return false;
        }

        m_instanceToken.reset();
        return true;
    }

private:
    std::optional<WpaDaemonInstanceHandle> m_instanceToken;
    const std::string_view m_wpaDaemonName;
    static constexpr Wpa::WpaType m_wpaType{ wpaType };
};

#endif // WPA_DAEMON_INSTANCE_HXX
