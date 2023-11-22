
#ifndef WPA_COMMAND_STATUS_HXX
#define WPA_COMMAND_STATUS_HXX

#include <Wpa/WpaCommand.hxx>

namespace Wpa
{
/**
 * @brief Representation of the "STATUS" command.
 */
struct WpaCommandStatus :
    public WpaCommand
{
    constexpr WpaCommandStatus();
};
} // namespace Wpa

#endif // WPA_COMMAND_STATUS_HXX
