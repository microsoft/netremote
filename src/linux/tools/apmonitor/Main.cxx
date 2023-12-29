
#include <format>

#include <magic_enum.hpp>
#include <microsoft/net/wifi/IAccessPoint.hxx>
#include <microsoft/net/wifi/AccessPointDiscoveryAgent.hxx>
#include <microsoft/net/wifi/AccessPointDiscoveryAgentOperationsNetlink.hxx>
#include <plog/Appenders/ColorConsoleAppender.h>
#include <plog/Formatters/MessageOnlyFormatter.h>
#include <plog/Init.h>
#include <plog/Log.h>
#include <signal.h>

using Microsoft::Net::Wifi::AccessPointDiscoveryAgent;
using Microsoft::Net::Wifi::IAccessPointDiscoveryAgentOperations;
using Microsoft::Net::Wifi::AccessPointDiscoveryAgentOperationsNetlink;

int
main([[maybe_unused]] int argc, [[maybe_unused]] char *argv[])
{
    // Configure console logging.
    static plog::ColorConsoleAppender<plog::MessageOnlyFormatter> colorConsoleAppender{};
    plog::init(plog::verbose, &colorConsoleAppender);

    // Configure monitoring with the netlink protocol.
    auto accessPointDiscoveryAgentOperationsNetlink{ std::make_unique<AccessPointDiscoveryAgentOperationsNetlink>() };
    auto accessPointDiscoveryAgent{ AccessPointDiscoveryAgent::Create(std::move(accessPointDiscoveryAgentOperationsNetlink)) };
    accessPointDiscoveryAgent->RegisterDiscoveryEventCallback([](auto&& presence, auto&& accessPointChanged) {
        PLOG_INFO << std::format("{} {}", magic_enum::enum_name(presence), accessPointChanged != nullptr ? accessPointChanged->GetInterface() : "<unknown>");
    });

    LOG_INFO << "starting access point discovery agent";
    accessPointDiscoveryAgent->Start();

    // Mask SIGTERM and SIGINT so they can be explicitly waited on from the main thread.
    int signal;
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGTERM);
    sigaddset(&mask, SIGINT);

    if (sigprocmask(SIG_BLOCK, &mask, nullptr) < 0) {
        LOG_ERROR << "failed to block terminate signals";
        return -1;
    }

    // Wait for the process to be signaled to exit.
    while (sigwait(&mask, &signal) != 0)
        ;

    // Received interrupt or terminate signal, so shut down.
    accessPointDiscoveryAgent->Stop();

    return 0;
}
