
#ifndef WPA_DAEMON_INSTANCE_HXX
#define WPA_DAEMON_INSTANCE_HXX

#include <Wpa/WpaCore.hxx>

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

    }

    void OnStopping() override
    {

    }

private:
    static constexpr Wpa::WpaType m_wpaType = wpaType;
};

#endif // WPA_DAEMON_INSTANCE_HXX
