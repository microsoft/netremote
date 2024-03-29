
#include <format>
#include <string_view>

#include <Wpa/ProtocolWpa.hxx>
#include <Wpa/WpaCommand.hxx>
#include <Wpa/WpaCommandSet.hxx>

using namespace Wpa;

WpaCommandSet::WpaCommandSet(std::string_view propertyName, std::string_view propertyValue) :
    PropertyPayload(std::format("{} {} {}", ProtocolWpa::CommandPayloadSet, propertyName, propertyValue)),
    PropertyName(propertyName),
    PropertyValue(propertyValue)
{
    SetPayload(PropertyPayload);
}
