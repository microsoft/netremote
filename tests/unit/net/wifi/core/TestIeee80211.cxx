
#include <initializer_list>

#include <microsoft/net/wifi/Ieee80211.hxx>

#include <catch2/catch_test_macros.hpp>

// Disable flagging magic numbers as the whole point of a user-defined literal is to be able to use a more descriptive
// literal in the code. 
// NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

TEST_CASE("IeeeDot11FrequencyBand GHz literals translate correctly", "[wifi][core]")
{
    using namespace Microsoft::Net::Wifi::Literals;

    using Microsoft::Net::Wifi::Ieee80211FrequencyBand;

    SECTION("Valid literal values are correct")
    {
        REQUIRE(2.4_GHz == Ieee80211FrequencyBand::TwoPointFourGHz);
        REQUIRE(5.0_GHz == Ieee80211FrequencyBand::FiveGHz);
        REQUIRE(6.0_GHz == Ieee80211FrequencyBand::SixGHz);
    }

    SECTION("Invalid literal values translate to 'Unknown'")
    {
        static const std::initializer_list<Ieee80211FrequencyBand> InvalidBandValues{
            0.0_GHz,
            1.0_GHz,
            2.0_GHz,
            2.3_GHz,
            2.39_GHz,
            2.44_GHz,
            4.0_GHz,
            4.99999_GHz,
            5.1_GHz,
            5.01_GHz,
            5.9999999999_GHz,
            6.01_GHz,
            6.000000000000001_GHz,
            7.0_GHz,
            2000.00_GHz,
            2400.00_GHz,
            5000.12_GHz,
            123456.7890_GHz,
        };

        for (const auto& invalidBand : InvalidBandValues) {
            REQUIRE(invalidBand == Ieee80211FrequencyBand::Unknown);
        }
    }
}

TEST_CASE("Ieee80211FrequencyBand MHz literals translate correctly", "[wifi][core]")
{
    using namespace Microsoft::Net::Wifi::Literals;

    using Microsoft::Net::Wifi::Ieee80211FrequencyBand;

    SECTION("Valid literal values are correct")
    {
        REQUIRE(2400_MHz == Ieee80211FrequencyBand::TwoPointFourGHz);
        REQUIRE(5000_MHz == Ieee80211FrequencyBand::FiveGHz);
        REQUIRE(6000_MHz == Ieee80211FrequencyBand::SixGHz);
    }

    SECTION("Invalid literal values translate to 'Unknown'")
    {
        static constexpr std::initializer_list<Ieee80211FrequencyBand> InvalidBandValues{
            0_MHz,
            1_MHz,
            2_MHz,
            4_MHz,
            7_MHz,
            2000_MHz,
            2399_MHz,
            2401_MHz,
            4999_MHz,
            5001_MHz,
            5999_MHz,
            6001_MHz,
            123456789_MHz,
            4294967295_MHz,
        };

        for (const auto& invalidBand : InvalidBandValues) {
            REQUIRE(invalidBand == Ieee80211FrequencyBand::Unknown);
        }
    }
}
