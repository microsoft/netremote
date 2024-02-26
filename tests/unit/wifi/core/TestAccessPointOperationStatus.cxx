
#include <optional>
#include <source_location>

#include <catch2/catch_message.hpp>
#include <catch2/catch_test_macros.hpp>
#include <magic_enum.hpp>
#include <microsoft/net/wifi/AccessPointOperationStatus.hxx>
#include <plog/Log.h>
#include <plog/Severity.h>

namespace Microsoft::Net::Wifi::Test
{
static constexpr auto AccessPointId{ "AP1" };
static constexpr auto OperationName{ "Operation1" };
static constexpr auto Details{ "Details1" };
} // namespace Microsoft::Net::Wifi::Test

TEST_CASE("Create an AccessPointOperationStatus instance", "[wifi][core][ap][log]")
{
    using namespace Microsoft::Net::Wifi;
    using namespace Microsoft::Net::Wifi::Test;

    SECTION("Create with args {accessPointId} doesn't cause a crash")
    {
        std::optional<AccessPointOperationStatus> status;
        REQUIRE_NOTHROW(status.emplace(AccessPointId));
    }

    SECTION("Create with args {accessPointId,operationName} doesn't cause a crash")
    {
        std::optional<AccessPointOperationStatus> status;
        REQUIRE_NOTHROW(status.emplace(AccessPointId, OperationName));
    }

    SECTION("Create with args {accessPointId,operationName,code[*]} doesn't cause a crash")
    {
        for (const auto statusCode : magic_enum::enum_values<AccessPointOperationStatusCode>()) {
            std::optional<AccessPointOperationStatus> status;
            REQUIRE_NOTHROW(status.emplace(AccessPointId, OperationName, statusCode));
        }
    }

    SECTION("Create with args {accessPointId,operationName,code[*],details} doesn't cause a crash")
    {
        for (const auto statusCode : magic_enum::enum_values<AccessPointOperationStatusCode>()) {
            std::optional<AccessPointOperationStatus> status;
            REQUIRE_NOTHROW(status.emplace(AccessPointId, OperationName, statusCode, Details));
        }
    }
}

TEST_CASE("AccessPointOperationStatus instance reflects basic properties", "[wifi][core][ap][log]")
{
    using namespace Microsoft::Net::Wifi;
    using namespace Microsoft::Net::Wifi::Test;

    SECTION("AccessPointId reflects the value passed to the constructor")
    {
        AccessPointOperationStatus status{ AccessPointId };
        REQUIRE(status.AccessPointId == AccessPointId);
    }

    SECTION("OperationName reflects the value passed to the constructor")
    {
        AccessPointOperationStatus status{ AccessPointId, OperationName };
        REQUIRE(status.OperationName == OperationName);
    }

    SECTION("Code reflects the value passed to the constructor")
    {
        for (const auto statusCode : magic_enum::enum_values<AccessPointOperationStatusCode>()) {
            AccessPointOperationStatus status{ AccessPointId, OperationName, statusCode };
            REQUIRE(status.Code == statusCode);
        }
    }

    SECTION("Details reflects the value passed to the constructor")
    {
        AccessPointOperationStatus status{ AccessPointId, OperationName, AccessPointOperationStatusCode::Succeeded, Details };
        REQUIRE(status.Details == Details);
    }
}

TEST_CASE("AccessPointOperationStatus instance reflects function name on empty operation name", "[wifi][core][ap][log]")
{
    using namespace Microsoft::Net::Wifi;
    using namespace Microsoft::Net::Wifi::Test;

    SECTION("OperationName reflects the function name when empty")
    {
        const AccessPointOperationStatus status{ AccessPointId };
        REQUIRE(status.OperationName.contains(__func__));
    }

    SECTION("OperationName does not reflect the function name when non-empty")
    {
        const AccessPointOperationStatus status{ AccessPointId, OperationName };
        REQUIRE_FALSE(status.OperationName.contains(__func__));
    }
}

TEST_CASE("AccessPointOperationStatus::MakeSucceeded returns a status with the correct code", "[wifi][core][ap][log]")
{
    using namespace Microsoft::Net::Wifi;
    using namespace Microsoft::Net::Wifi::Test;

    SECTION("MakeSucceeded returns a status with the Succeeded code")
    {
        const auto status = AccessPointOperationStatus::MakeSucceeded(AccessPointId);
        REQUIRE(status.Code == AccessPointOperationStatusCode::Succeeded);
    }
}

TEST_CASE("AccessPointOperationStatus::Succeeded() reflects the status code", "[wifi][core][ap][log]")
{
    using namespace Microsoft::Net::Wifi;
    using namespace Microsoft::Net::Wifi::Test;

    SECTION("Returns 'true' for a status with the Succeeded code")
    {
        const auto status = AccessPointOperationStatus::MakeSucceeded(AccessPointId);
        REQUIRE(status.Succeeded());
    }

    SECTION("Returns 'false' for a status with a code other than Succeeded")
    {
        for (const auto statusCode : magic_enum::enum_values<AccessPointOperationStatusCode>()) {
            if (statusCode != AccessPointOperationStatusCode::Succeeded) {
                const auto status = AccessPointOperationStatus{ AccessPointId, OperationName, statusCode };
                REQUIRE_FALSE(status.Succeeded());
            }
        }
    }
}

TEST_CASE("AccessPointOperationStatus::Failed() reflects the status code", "[wifi][core][ap][log]")
{
    using namespace Microsoft::Net::Wifi;
    using namespace Microsoft::Net::Wifi::Test;

    SECTION("Returns 'true' for a status with a code other than Succeeded")
    {
        for (const auto statusCode : magic_enum::enum_values<AccessPointOperationStatusCode>()) {
            if (statusCode != AccessPointOperationStatusCode::Succeeded) {
                const auto status = AccessPointOperationStatus{ AccessPointId, OperationName, statusCode };
                REQUIRE(status.Failed());
            }
        }
    }

    SECTION("Returns 'false' for a status with the Succeeded code")
    {
        const auto status = AccessPointOperationStatus::MakeSucceeded(AccessPointId);
        REQUIRE_FALSE(status.Failed());
    }
}

TEST_CASE("AccessPointOperationStatus::ToString() includes all properties", "[wifi][core][ap][log]")
{
    using namespace Microsoft::Net::Wifi;
    using namespace Microsoft::Net::Wifi::Test;

    SECTION("Returns a string with all properties")
    {
        for (const auto statusCode : magic_enum::enum_values<AccessPointOperationStatusCode>()) {
            const auto status = AccessPointOperationStatus{ AccessPointId, OperationName, statusCode, Details };
            const auto statusMessage = status.ToString();
            REQUIRE(statusMessage.contains(AccessPointId));
            REQUIRE(statusMessage.contains(OperationName));
            REQUIRE(statusMessage.contains(Details));
            if (statusCode != AccessPointOperationStatusCode::Succeeded) {
                REQUIRE(statusMessage.contains(magic_enum::enum_name(statusCode)));
            }
        }
    }
}

TEST_CASE("AccessPointOperationStatus::ToString() result can be printed", "[wifi][core][ap][log]")
{
    using namespace Microsoft::Net::Wifi;
    using namespace Microsoft::Net::Wifi::Test;

    SECTION("Returns a string with all properties")
    {
        for (const auto statusCode : magic_enum::enum_values<AccessPointOperationStatusCode>()) {
            const auto status = AccessPointOperationStatus{ AccessPointId, OperationName, statusCode, Details };
            const auto logSeverity = status.Succeeded() ? plog::debug : plog::error;
            REQUIRE_NOTHROW([&]() {
                LOG(logSeverity) << status.ToString();
            }());
        }
    }
}
