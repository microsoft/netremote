
#include <filesystem>
#include <string_view>
#include <utility>

#include <Wpa/WpaControlSocket.hxx>
#include <Wpa/WpaControlSocketConnection.hxx>

using namespace Wpa;

/* static */
bool
WpaControlSocket::Exists(std::string_view interfaceName, std::filesystem::path controlSocketPath)
{
    // Attempt to create a control socket connection to the specified path and interface. If it succeeds, the path is
    // valid. There's no real way around this without trying to connect to the control socket since calling open() on it
    // will fail even if the socket is valid and accessible.
    const auto wpaControlSocketConnection = WpaControlSocketConnection::TryCreate(interfaceName, std::move(controlSocketPath));
    return wpaControlSocketConnection != nullptr;
}

/* static */
bool
WpaControlSocket::Exists(std::string_view interfaceName, WpaType wpaType)
{
    return Exists(interfaceName, DefaultPath(wpaType));
}
