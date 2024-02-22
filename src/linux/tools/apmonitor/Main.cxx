
#include <csignal> // NOLINT(misc-include-cleaner)
#include <format>
#include <memory>
#include <utility>

#include <magic_enum.hpp>
#include <microsoft/net/wifi/AccessPointControllerLinux.hxx>
#include <microsoft/net/wifi/AccessPointDiscoveryAgent.hxx>
#include <microsoft/net/wifi/AccessPointDiscoveryAgentOperationsNetlink.hxx>
#include <microsoft/net/wifi/AccessPointLinux.hxx>
#include <plog/Appenders/ColorConsoleAppender.h>
#include <plog/Formatters/MessageOnlyFormatter.h>
#include <plog/Init.h>
#include <plog/Log.h>
#include <plog/Severity.h>

using namespace Microsoft::Net::Wifi;

int
main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[])
{
    // Configure console logging.
    static plog::ColorConsoleAppender<plog::MessageOnlyFormatter> colorConsoleAppender{};
    plog::init(plog::verbose, &colorConsoleAppender);

    // Configure monitoring with the netlink protocol.
    auto accessPointControllerFactory = std::make_unique<AccessPointControllerLinuxFactory>();
    auto accessPointFactory = std::make_shared<AccessPointFactoryLinux>(std::move(accessPointControllerFactory));
    auto accessPointDiscoveryAgentOperationsNetlink{ std::make_unique<AccessPointDiscoveryAgentOperationsNetlink>(accessPointFactory) };
    auto accessPointDiscoveryAgent{ AccessPointDiscoveryAgent::Create(std::move(accessPointDiscoveryAgentOperationsNetlink)) };
    accessPointDiscoveryAgent->RegisterDiscoveryEventCallback([](auto&& presence, auto&& accessPointChanged) {
        LOGI << std::format("{} -> {}", accessPointChanged->GetInterfaceName(), magic_enum::enum_name(presence));
    });

    LOGI << "starting access point discovery agent";
    accessPointDiscoveryAgent->Start();

    // Mask SIGTERM and SIGINT so they can be explicitly waited on from the main thread.
    int signal;
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGTERM);
    sigaddset(&mask, SIGINT);

    if (sigprocmask(SIG_BLOCK, &mask, nullptr) < 0) {
        LOGE << "failed to block terminate signals";
        return -1;
    }

    // Wait for the process to be signaled to exit.
    while (sigwait(&mask, &signal) != 0)
        ;

    // Received interrupt or terminate signal, so shut down.
    accessPointDiscoveryAgent->Stop();

    return 0;
}
