
#include <filesystem>
#include <string_view>

#include <Wpa/WpaControlSocket.hxx>
#include <Wpa/WpaControlSocketConnection.hxx>

using namespace Wpa;

/* static */
bool
WpaControlSocket::IsPathValidForInterface(std::filesystem::path controlSocketPath, std::string_view interfaceName)
{
    // Attempt to create a control socket connection to the specified path and interface. If it succeeds, the path is
    // valid. There's no real way around this without trying to connect to the control socket since calling open() on it
    // will fail even if the socket is valid and accessible.
    const auto wpaControlSocketConnection = WpaControlSocketConnection::TryCreate(std::move(controlSocketPath), interfaceName);
    return wpaControlSocketConnection != nullptr;
}
