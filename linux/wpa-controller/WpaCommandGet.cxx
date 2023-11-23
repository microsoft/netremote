
#include <format>
#include <string>

#include <Wpa/WpaCommandGet.hxx>

using namespace Wpa;

WpaCommandGet::WpaCommandGet(std::string_view propertyName) :
    WpaCommand(std::string(ProtocolWpa::CommandPayloadGet).append(std::format(" {}", propertyName))),
    PropertyName(propertyName)
{
}
