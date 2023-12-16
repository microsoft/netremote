
#include <Wpa/ProtocolWpa.hxx>
#include <Wpa/WpaResponse.hxx>

using namespace Wpa;

WpaResponse::WpaResponse(std::string_view payload) :
    m_payload(payload),
    Payload(m_payload)
{
}

WpaResponse::operator bool() const
{
    return IsOk();
}

bool
WpaResponse::IsOk() const
{
    return (ProtocolWpa::IsResponseOk(Payload));
}

bool
WpaResponse::Failed() const
{
    return (ProtocolWpa::IsResponseFail(Payload));
}
