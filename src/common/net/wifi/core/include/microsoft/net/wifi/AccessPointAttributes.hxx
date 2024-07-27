
#ifndef ACCESS_POINT_ATTRIBUTES_HXX
#define ACCESS_POINT_ATTRIBUTES_HXX

#include <string>
#include <unordered_map>

namespace Microsoft::Net::Wifi
{
/**
 * @brief Container to hold static attributes about an access point.
 */
struct AccessPointAttributes
{
    std::unordered_map<std::string, std::string> Properties{};
};
} // namespace Microsoft::Net::Wifi

#endif // ACCESS_POINT_ATTRIBUTES_HXX
