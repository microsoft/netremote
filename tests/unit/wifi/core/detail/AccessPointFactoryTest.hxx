
#ifndef ACCESS_POIINT_FACTORY_TEST_HXX
#define ACCESS_POIINT_FACTORY_TEST_HXX 

#include <memory>
#include <string_view>

#include <microsoft/net/wifi/AccessPoint.hxx>
#include <microsoft/net/wifi/IAccessPointFactory.hxx>

namespace Microsoft::Net::Wifi::Test
{
/**
 * @brief Basic factory implementation to use for testing purposes. This
 * implementation creates AccessPoints that perform no-ops.
 */
struct AccessPointFactoryTest :
    public Microsoft::Net::Wifi::IAccessPointFactory
{
    AccessPointFactoryTest() = default;
    ~AccessPointFactoryTest() = default;

    /**
     * @brief Create an AccessPoint for testing purposes.
     * 
     * @param interface The interface to create the AccessPoint for. This can be
     * any string and does not have to correspond to a real device interface.
     * 
     * @return std::shared_ptr<AccessPoint> 
     */
    std::shared_ptr<AccessPoint> Create(std::string_view interface) override;
};
} // namespace Microsoft::Net::Wifi::Test

#endif // ACCESS_POIINT_FACTORY_TEST_HXX
