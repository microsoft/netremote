
#ifndef WPA_EVENT_HXX
#define WPA_EVENT_HXX

#include <optional>
#include <string>
#include <string_view>

#include <Wpa/WpaCore.hxx>

namespace Wpa
{
/**
 * @brief Represents an event from a WPA daemon/service.
 */
struct WpaEvent
{
    WpaType Source{ WpaType::Unknown };
    WpaLogLevel LogLevel{ WpaLogLevel::Unknown };
    std::string Payload{};
    std::optional<std::string> Interface{ std::nullopt };

    /**
     * @brief Parse a string into a WpaEvent.
     *
     * @param eventPayload The string to parse.
     * @return std::optional<WpaEvent>
     */
    static std::optional<WpaEvent>
    Parse(std::string_view eventPayload);

    /**
     * @brief Convert the event to a string.
     *
     * @return std::string
     */
    std::string
    ToString() const;
};

} // namespace Wpa

#endif // WPA_EVENT_HXX
