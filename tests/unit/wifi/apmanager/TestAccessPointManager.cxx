
#include <catch2/catch_test_macros.hpp>
#include <microsoft/net/wifi/AccessPointDiscoveryAgent.hxx>
#include <microsoft/net/wifi/AccessPointManager.hxx>

#include "AccessPointDiscoveryAgentOperationsTest.hxx"
#include "AccessPointTest.hxx"

TEST_CASE("Create an AccessPointManager instance", "[wifi][core][apmanager]")
{
    using namespace Microsoft::Net::Wifi;

    SECTION("Create doesn't cause a crash")
    {
        REQUIRE_NOTHROW(AccessPointManager::Create());
    }
}

TEST_CASE("Destroy an AccessPointManager instance", "[wifi][core][apmanager]")
{
    using namespace Microsoft::Net::Wifi;

    using Test::AccessPointDiscoveryAgentOperationsTest;

    SECTION("Destroy doesn't cause a crash")
    {
        auto accessPointManager{ AccessPointManager::Create() };
        REQUIRE_NOTHROW(accessPointManager.reset());
    }

    SECTION("Destroy doesn't cause a crash when there are registered discovery agents")
    {
        auto accessPointDiscoveryAgent1{ AccessPointDiscoveryAgent::Create(std::make_unique<AccessPointDiscoveryAgentOperationsTest>()) };
        auto accessPointDiscoveryAgent2{ AccessPointDiscoveryAgent::Create(std::make_unique<AccessPointDiscoveryAgentOperationsTest>()) };

        auto accessPointManager{ AccessPointManager::Create() };
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
        auto accessPointManager{ AccessPointManager::Create() };
        REQUIRE(std::empty(accessPointManager->GetAllAccessPoints()));
    }
}

TEST_CASE("AccessPointManager correctly manages lifetime of access points reported by discovery agents", "[wifi][core][apmanager]")
{
    using namespace Microsoft::Net::Wifi;
    using Test::AccessPointDiscoveryAgentOperationsTest;

    auto accessPoint1{ std::make_shared<Test::AccessPointTest>("accessPointTest1") };
    auto accessPoint2{ std::make_shared<Test::AccessPointTest>("accessPointTest2") };

    auto accessPointDiscoveryAgentOperationsTest{ std::make_unique<AccessPointDiscoveryAgentOperationsTest>() };
    auto* accessPointDiscoveryAgentOperationsTestPtr{ accessPointDiscoveryAgentOperationsTest.get() };
    auto accessPointDiscoveryAgentTest{ AccessPointDiscoveryAgent::Create(std::move(accessPointDiscoveryAgentOperationsTest)) };

    auto accessPointManager{ AccessPointManager::Create() };
    accessPointManager->AddDiscoveryAgent(std::move(accessPointDiscoveryAgentTest));

    SECTION("Adds reference to discovered access points")
    {
        const auto referenceCount{ accessPoint1.use_count() };
        accessPointDiscoveryAgentOperationsTestPtr->AddAccessPoint(accessPoint1);
        REQUIRE(accessPoint1.use_count() > referenceCount);
    }

    SECTION("Removes reference from departed access points")
    {
        accessPointDiscoveryAgentOperationsTestPtr->AddAccessPoint(accessPoint1);
        const auto referenceCount{ accessPoint1.use_count() };
        accessPointDiscoveryAgentOperationsTestPtr->RemoveAccessPoint(accessPoint1);
        REQUIRE(accessPoint1.use_count() < referenceCount);
    }
}

TEST_CASE("AccessPointManager persists access points reported by discovery agents", "[wifi][core][apmanager]")
{
    using namespace Microsoft::Net::Wifi;
    using Test::AccessPointDiscoveryAgentOperationsTest;

    auto accessPoint1{ std::make_shared<Test::AccessPointTest>("accessPointTest1") };
    auto accessPoint2{ std::make_shared<Test::AccessPointTest>("accessPointTest2") };

    auto accessPointDiscoveryAgentOperationsTest{ std::make_unique<AccessPointDiscoveryAgentOperationsTest>() };
    auto* accessPointDiscoveryAgentOperationsTestPtr{ accessPointDiscoveryAgentOperationsTest.get() };
    auto accessPointDiscoveryAgentTest{ AccessPointDiscoveryAgent::Create(std::move(accessPointDiscoveryAgentOperationsTest)) };

    auto accessPointManager{ AccessPointManager::Create() };
    accessPointManager->AddDiscoveryAgent(std::move(accessPointDiscoveryAgentTest));

    SECTION("Access points from arrival events are persisted via GetAccessPoint()")
    {
        accessPointDiscoveryAgentOperationsTestPtr->AddAccessPoint(accessPoint1);

        auto accessPointRetrievedWeak{ accessPointManager->GetAccessPoint(accessPoint1->GetInterface()) };
        auto accessPointRetrieved{ accessPointRetrievedWeak.lock() };
        REQUIRE(accessPointRetrieved != nullptr);
        REQUIRE(accessPointRetrieved->GetInterface() == accessPoint1->GetInterface());
    }

    SECTION("Access points from arrival events are persisted via GetAllAccessPoints()")
    {
        accessPointDiscoveryAgentOperationsTestPtr->AddAccessPoint(accessPoint1);

        const auto accessPointsAll{ accessPointManager->GetAllAccessPoints() };
        REQUIRE(std::size(accessPointsAll) == 1);

        std::weak_ptr<IAccessPoint> accessPointRetrievedWeak{};
        REQUIRE_NOTHROW(accessPointRetrievedWeak = accessPointsAll[0]);
        auto accessPointRetrieved{ accessPointRetrievedWeak.lock() };
        REQUIRE(accessPointRetrieved != nullptr);
        REQUIRE(accessPointRetrieved->GetInterface() == accessPoint1->GetInterface());
    }

    SECTION("Access points from arrival events are persisted via GetAccessPoint() after additional access points have been added")
    {
        accessPointDiscoveryAgentOperationsTestPtr->AddAccessPoint(accessPoint1);
        accessPointDiscoveryAgentOperationsTestPtr->AddAccessPoint(accessPoint2);

        auto accessPointRetrievedWeak{ accessPointManager->GetAccessPoint(accessPoint1->GetInterface()) };
        auto accessPointRetrieved{ accessPointRetrievedWeak.lock() };
        REQUIRE(accessPointRetrieved != nullptr);
        REQUIRE(accessPointRetrieved->GetInterface() == accessPoint1->GetInterface());
    }

    SECTION("Access points from arrival events are persisted via GetAllAccessPoints() after additional access points have been added")
    {
        accessPointDiscoveryAgentOperationsTestPtr->AddAccessPoint(accessPoint1);
        accessPointDiscoveryAgentOperationsTestPtr->AddAccessPoint(accessPoint2);

        const auto accessPointsAll{ accessPointManager->GetAllAccessPoints() };
        REQUIRE(std::size(accessPointsAll) == 2);

        std::weak_ptr<IAccessPoint> accessPointRetrievedWeak{};
        REQUIRE_NOTHROW(accessPointRetrievedWeak = accessPointsAll[0]);
        auto accessPointRetrieved{ accessPointRetrievedWeak.lock() };
        REQUIRE(accessPointRetrieved != nullptr);
        REQUIRE(accessPointRetrieved->GetInterface() == accessPoint1->GetInterface());
    }
}

TEST_CASE("AccessPointManager discards access points reported to have departed by discovery agents", "[wifi][core][apmanager]")
{
    using namespace Microsoft::Net::Wifi;
    using Test::AccessPointDiscoveryAgentOperationsTest;

    auto accessPoint1{ std::make_shared<Test::AccessPointTest>("accessPointTest1") };
    auto accessPoint2{ std::make_shared<Test::AccessPointTest>("accessPointTest2") };

    auto accessPointDiscoveryAgentOperationsTest{ std::make_unique<AccessPointDiscoveryAgentOperationsTest>() };
    auto* accessPointDiscoveryAgentOperationsTestPtr{ accessPointDiscoveryAgentOperationsTest.get() };
    auto accessPointDiscoveryAgentTest{ AccessPointDiscoveryAgent::Create(std::move(accessPointDiscoveryAgentOperationsTest)) };

    auto accessPointManager{ AccessPointManager::Create() };
    accessPointManager->AddDiscoveryAgent(std::move(accessPointDiscoveryAgentTest));

    SECTION("Access points reported to have departed are not accessible via GetAccessPoint()")
    {
        accessPointDiscoveryAgentOperationsTestPtr->AddAccessPoint(accessPoint1);
        auto accessPointWeak{ accessPointManager->GetAccessPoint(accessPoint1->GetInterface()) };
        auto accessPoint{ accessPointWeak.lock() };

        accessPointDiscoveryAgentOperationsTestPtr->RemoveAccessPoint(accessPoint);
        auto accessPointRetrieved{ accessPointManager->GetAccessPoint(accessPoint1->GetInterface()) };
        REQUIRE(accessPointRetrieved.expired());
    }

    SECTION("Access points reported to have departed are not accessible via GetAllAccessPoints()")
    {
        accessPointDiscoveryAgentOperationsTestPtr->AddAccessPoint(accessPoint1);

        auto accessPointWeak{ accessPointManager->GetAccessPoint(accessPoint1->GetInterface()) };
        auto accessPoint{ accessPointWeak.lock() };

        accessPointDiscoveryAgentOperationsTestPtr->RemoveAccessPoint(accessPoint);

        const auto accessPointsAll{ accessPointManager->GetAllAccessPoints() };
        REQUIRE(std::empty(accessPointsAll));
    }
}
