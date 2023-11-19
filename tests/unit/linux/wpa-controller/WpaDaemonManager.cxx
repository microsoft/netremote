
#include "WpaDaemonManager.hxx"

/* static */
std::optional<WpaDaemonInstanceToken> WpaDaemonManager::Start([[maybe_unused]] Wpa::WpaType wpaType)
{
    return std::nullopt;
}

/* static */
bool WpaDaemonManager::Stop([[maybe_unused]] const WpaDaemonInstanceToken& instanceToken)
{
    return false;
}
