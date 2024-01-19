
#include <string_view>

#include <catch2/catch_test_macros.hpp>
#include <microsoft/net/wifi/AccessPointDiscoveryAgent.hxx>
#include <microsoft/net/wifi/AccessPointManager.hxx>
#include <microsoft/net/wifi/test/AccessPointControllerTest.hxx>
#include <microsoft/net/wifi/test/AccessPointTest.hxx>

#include "AccessPointDiscoveryAgentOperationsTest.hxx"

TEST_CASE("Create an AccessPointManager instance", "[wifi][core][apmanager]")
{
    using namespace Microsoft::Net::Wifi;

    SECTION("Create doesn't cause a crash")
    {
        REQUIRE_NOTHROW(AccessPointManager::Create(std::make_unique<Test::AccessPointFactoryTest>()));
    }
}

TEST_CASE("Destroy an AccessPointManager instance", "[wifi][core][apmanager]")
{
    using namespace Microsoft::Net::Wifi;

    using Test::AccessPointDiscoveryAgentOperationsTest;

    SECTION("Destroy doesn't cause a crash")
    {
        auto accessPointManager{ AccessPointManager::Create(std::make_unique<Test::AccessPointFactoryTest>()) };
        REQUIRE_NOTHROW(accessPointManager.reset());
    }

    SECTION("Destroy doesn't cause a crash when there are registered discovery agents")
    {
        auto accessPointDiscoveryAgent1{ AccessPointDiscoveryAgent::Create(std::make_unique<AccessPointDiscoveryAgentOperationsTest>()) };
        auto accessPointDiscoveryAgent2{ AccessPointDiscoveryAgent::Create(std::make_unique<AccessPointDiscoveryAgentOperationsTest>()) };

        auto accessPointManager{ AccessPointManager::Create(std::make_unique<Test::AccessPointFactoryTest>()) };
        accessPointManager->AddDiscoveryAgent(std::move(accessPointDiscoveryAgent1));
        accessPointManager->AddDiscoveryAgent(std::move(accessPointDiscoveryAgent2));
        REQUIRE_NOTHROW(accessPointManager.reset());
    }
}

TEST_CASE("AccessPointManager instance reflects basic properties", "[wifi][core][apmanager]")
{
    using namespace Microsoft::Net::Wifi;

    SECTION("GetAllAccessPoints() returns an empty list")
    {
        auto accessPointManager{ AccessPointManager::Create(std::make_unique<Test::AccessPointFactoryTest>()) };
        REQUIRE(std::empty(accessPointManager->GetAllAccessPoints()));
    }
}

TEST_CASE("AccessPointManager persists access points reported by discovery agents", "[wifi][core][apmanager]")
{
    using namespace Microsoft::Net::Wifi;
    using Test::AccessPointDiscoveryAgentOperationsTest;

    std::string_view accessPointInterfaceName1{ "accessPointTest1" };
    std::string_view accessPointInterfaceName2{ "accessPointTest2" };

    auto accessPointDiscoveryAgentOperationsTest{ std::make_unique<AccessPointDiscoveryAgentOperationsTest>() };
    auto* accessPointDiscoveryAgentOperationsTestPtr{ accessPointDiscoveryAgentOperationsTest.get() };
    auto accessPointDiscoveryAgentTest{ AccessPointDiscoveryAgent::Create(std::move(accessPointDiscoveryAgentOperationsTest)) };

    auto accessPointManager{ AccessPointManager::Create(std::make_unique<Test::AccessPointFactoryTest>()) };
    accessPointManager->AddDiscoveryAgent(std::move(accessPointDiscoveryAgentTest));

    SECTION("Access points from arrival events are persisted via GetAccessPoint()")
    {
        accessPointDiscoveryAgentOperationsTestPtr->AddAccessPoint(accessPointInterfaceName1);

        auto accessPointRetrievedWeak{ accessPointManager->GetAccessPoint(accessPointInterfaceName1) };
        auto accessPointRetrieved{ accessPointRetrievedWeak.lock() };
        REQUIRE(accessPointRetrieved != nullptr);
        REQUIRE(accessPointRetrieved->GetInterfaceName() == accessPointInterfaceName1);
    }

    SECTION("Access points from arrival events are persisted via GetAllAccessPoints()")
    {
        accessPointDiscoveryAgentOperationsTestPtr->AddAccessPoint(accessPointInterfaceName1);

        const auto accessPointsAll{ accessPointManager->GetAllAccessPoints() };
        REQUIRE(std::size(accessPointsAll) == 1);

        std::weak_ptr<IAccessPoint> accessPointRetrievedWeak{};
        REQUIRE_NOTHROW(accessPointRetrievedWeak = accessPointsAll[0]);
        auto accessPointRetrieved{ accessPointRetrievedWeak.lock() };
        REQUIRE(accessPointRetrieved != nullptr);
        REQUIRE(accessPointRetrieved->GetInterfaceName() == accessPointInterfaceName1);
    }

    SECTION("Access points from arrival events are persisted via GetAccessPoint() after additional access points have been added")
    {
        accessPointDiscoveryAgentOperationsTestPtr->AddAccessPoint(accessPointInterfaceName1);
        accessPointDiscoveryAgentOperationsTestPtr->AddAccessPoint(accessPointInterfaceName2);

        auto accessPointRetrievedWeak{ accessPointManager->GetAccessPoint(accessPointInterfaceName1) };
        auto accessPointRetrieved{ accessPointRetrievedWeak.lock() };
        REQUIRE(accessPointRetrieved != nullptr);
        REQUIRE(accessPointRetrieved->GetInterfaceName() == accessPointInterfaceName1);
    }

    SECTION("Access points from arrival events are persisted via GetAllAccessPoints() after additional access points have been added")
    {
        accessPointDiscoveryAgentOperationsTestPtr->AddAccessPoint(accessPointInterfaceName1);
        accessPointDiscoveryAgentOperationsTestPtr->AddAccessPoint(accessPointInterfaceName2);

        const auto accessPointsAll{ accessPointManager->GetAllAccessPoints() };
        REQUIRE(std::size(accessPointsAll) == 2);

        std::weak_ptr<IAccessPoint> accessPointRetrievedWeak{};
        REQUIRE_NOTHROW(accessPointRetrievedWeak = accessPointsAll[0]);
        auto accessPointRetrieved{ accessPointRetrievedWeak.lock() };
        REQUIRE(accessPointRetrieved != nullptr);
        REQUIRE(accessPointRetrieved->GetInterfaceName() == accessPointInterfaceName1);
    }
}

TEST_CASE("AccessPointManager discards access points reported to have departed by discovery agents", "[wifi][core][apmanager]")
{
    using namespace Microsoft::Net::Wifi;
    using Test::AccessPointDiscoveryAgentOperationsTest;

    std::string_view accessPointInterfaceName{ "accessPointTest" };

    auto accessPointDiscoveryAgentOperationsTest{ std::make_unique<AccessPointDiscoveryAgentOperationsTest>() };
    auto* accessPointDiscoveryAgentOperationsTestPtr{ accessPointDiscoveryAgentOperationsTest.get() };
    auto accessPointDiscoveryAgentTest{ AccessPointDiscoveryAgent::Create(std::move(accessPointDiscoveryAgentOperationsTest)) };

    auto accessPointManager{ AccessPointManager::Create(std::make_unique<Test::AccessPointFactoryTest>()) };
    accessPointManager->AddDiscoveryAgent(std::move(accessPointDiscoveryAgentTest));

    SECTION("Access points reported to have departed are not accessible via GetAccessPoint()")
    {
        accessPointDiscoveryAgentOperationsTestPtr->AddAccessPoint(accessPointInterfaceName);
        auto accessPointWeak{ accessPointManager->GetAccessPoint(accessPointInterfaceName) };
        auto accessPoint{ accessPointWeak.lock() };

        accessPointDiscoveryAgentOperationsTestPtr->RemoveAccessPoint(accessPointInterfaceName);
        auto accessPointRetrieved{ accessPointManager->GetAccessPoint(accessPointInterfaceName) };
        REQUIRE(accessPointRetrieved.expired());
    }

    SECTION("Access points reported to have departed are not accessible via GetAllAccessPoints()")
    {
        accessPointDiscoveryAgentOperationsTestPtr->AddAccessPoint(accessPointInterfaceName);

        auto accessPointWeak{ accessPointManager->GetAccessPoint(accessPointInterfaceName) };
        auto accessPoint{ accessPointWeak.lock() };

        accessPointDiscoveryAgentOperationsTestPtr->RemoveAccessPoint(accessPointInterfaceName);

        const auto accessPointsAll{ accessPointManager->GetAllAccessPoints() };
        REQUIRE(std::empty(accessPointsAll));
    }
}
