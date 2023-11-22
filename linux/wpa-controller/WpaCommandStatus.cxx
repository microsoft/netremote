
#include <Wpa/WpaCommandStatus.hxx>
#include <Wpa/ProtocolWpa.hxx>

using namespace Wpa;

constexpr WpaCommandStatus::WpaCommandStatus() :
    WpaCommand(ProtocolWpa::CommandPayloadStatus)
{
}
