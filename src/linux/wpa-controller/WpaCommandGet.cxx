
#include <format>

#include <Wpa/WpaCommandGet.hxx>

using namespace Wpa;

WpaCommandGet::WpaCommandGet(std::string_view propertyName) :
    WpaCommand(),
    PropertyPayload(std::format("{} {}", ProtocolWpa::CommandPayloadGet, propertyName)),
    PropertyName(propertyName)
{
    SetPayload(PropertyPayload);
}
