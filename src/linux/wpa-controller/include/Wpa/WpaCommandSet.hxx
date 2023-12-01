
#ifndef WPA_COMMAND_SET_HXX
#define WPA_COMMAND_SET_HXX

#include <string_view>
#include <string>

#include <Wpa/WpaCommand.hxx>

namespace Wpa
{
/**
 * @brief Representation of the "SET" command.
 */
struct WpaCommandSet :
    public WpaCommand
{
    /**
     * @brief Construct a new WpaCommandSet object for the specified property.
     * 
     * @param propertyName The name of the property to set.
     * @param propertyValue The value to set the property to.
     */
    WpaCommandSet(std::string_view propertyName, std::string_view propertyValue);

    std::string PropertyPayload;
    std::string_view PropertyName;
    std::string_view PropertyValue;
};

} // namespace Wpa

#endif //  WPA_COMMAND_SET_HXX
