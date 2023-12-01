
#include <Wpa/ProtocolWpa.hxx>

using namespace Wpa;

/* static */
bool ProtocolWpa::IsResponseOk(std::string_view response)
{
    return (response.starts_with(ResponsePayloadOk));
}

/* static */
bool ProtocolWpa::IsResponseFail(std::string_view response)
{
    return (response.starts_with(ResponsePayloadFail));
}
