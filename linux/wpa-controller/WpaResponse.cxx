
#include <Wpa/WpaResponse.hxx>

using namespace Wpa;

WpaResponse::WpaResponse(std::string_view payload) :
    Payload(payload)
{
}
