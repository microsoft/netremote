
#ifndef WPA_COMMAND_GET_HXX
#define WPA_COMMAND_GET_HXX

#include <string>
#include <string_view>

#include <Wpa/WpaCommand.hxx>

namespace Wpa
{
/**
 * @brief Representation of the "GET" command.
 */
struct WpaCommandGet :
    public WpaCommand
{
    /**
     * @brief Construct a new WpaCommandGet object for the specified property.
     *
     * @param propertyName The name of the property to retrieve.
     */
    WpaCommandGet(std::string_view propertyName);

    std::string PropertyPayload;
    std::string_view PropertyName;
};

} // namespace Wpa

#endif //  WPA_COMMAND_GET_HXX
