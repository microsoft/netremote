
#ifndef WPA_DAEMON_INSTANCE_HXX
#define WPA_DAEMON_INSTANCE_HXX

#include <format>

#include <Wpa/WpaCore.hxx>
#include "WpaDaemonManager.hxx"

struct IWpaDaemonInstance
{
    virtual ~IWpaDaemonInstance() = default;

    virtual void OnStarting() = 0;
    virtual void OnStopping() = 0;
};

template <Wpa::WpaType wpaType>
struct WpaDaemonInstance : public IWpaDaemonInstance
{
    void OnStarting() override
    {
        auto instanceToken = WpaDaemonManager::Start(m_wpaType);
        if (!instanceToken.has_value())
        {
            std::cerr << std::format("Failed to start wpa {} daemon instance", magic_enum::enum_name(m_wpaType));
            return;
        }

        m_instanceToken = std::move(instanceToken);
    }

    void OnStopping() override
    {
        if (!m_instanceToken.has_value())
        {
            std::cerr << std::format("Failed to stop wpa {} daemon instance (not started)", magic_enum::enum_name(m_wpaType));
            return;
        }

        if (!WpaDaemonManager::Stop(m_instanceToken.value()))
        {
            std::cerr << std::format("Failed to stop wpa {} daemon instance", magic_enum::enum_name(m_wpaType));
            return;
        }

        m_instanceToken.reset();
    }

private:
    std::optional<WpaDaemonInstanceToken> m_instanceToken;
    static constexpr Wpa::WpaType m_wpaType{ wpaType };
};

#endif // WPA_DAEMON_INSTANCE_HXX
