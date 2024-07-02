
#include <string_view>

#include <Wpa/IHostapd.hxx>

using namespace Wpa;

HostapdException::HostapdException(std::string_view message) :
    m_message(message)
{
}

const char*
HostapdException::what() const noexcept
{
    return m_message.c_str();
}
