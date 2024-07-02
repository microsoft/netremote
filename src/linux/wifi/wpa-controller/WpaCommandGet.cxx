
#include <format>
#include <string_view>

#include <Wpa/ProtocolWpa.hxx>
#include <Wpa/WpaCommandGet.hxx>

using namespace Wpa;

WpaCommandGet::WpaCommandGet(std::string_view propertyName) :
    PropertyPayload(std::format("{} {}", ProtocolWpa::CommandPayloadGet, propertyName)),
    PropertyName(propertyName)
{
    SetPayload(PropertyPayload);
}
