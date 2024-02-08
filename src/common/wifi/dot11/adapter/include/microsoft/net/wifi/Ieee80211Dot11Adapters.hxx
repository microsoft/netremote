
#ifndef IEEE_80211_DOT11_ADAPTERS_HXX
#define IEEE_80211_DOT11_ADAPTERS_HXX

#include <microsoft/net/remote/protocol/NetRemoteService.grpc.pb.h>
#include <microsoft/net/wifi/Ieee80211.hxx>

namespace Microsoft::Net::Wifi
{
/**
 * @brief Convert the specified Dot11PhyType to the equivalent IEEE 802.11 protocol.
 *
 * @param ieeeProtocol The IEEE 802.11 protocol to convert.
 * @return Microsoft::Net::Wifi::Dot11PhyType
 */
Microsoft::Net::Wifi::Dot11PhyType
ToDot11PhyType(const Microsoft::Net::Wifi::Ieee80211Protocol ieeeProtocol) noexcept;

/**
 * @brief Convert the specified Dot11PhyType to the equivalent IEEE 802.11 protocol.
 *
 * @param dot11PhyType The Dot11PhyType to convert.
 * @return Microsoft::Net::Wifi::Ieee80211Protocol
 */
Microsoft::Net::Wifi::Ieee80211Protocol
FromDot11PhyType(const Microsoft::Net::Wifi::Dot11PhyType dot11PhyType) noexcept;

/**
 * @brief Convert the specified Dot11FrequencyBand to the equivalent IEEE 802.11 frequency band.
 *
 * @param ieeeFrequencyBand The IEEE 802.11 frequency band to convert.
 * @return Microsoft::Net::Wifi::Dot11FrequencyBand
 */
Microsoft::Net::Wifi::Dot11FrequencyBand
ToDot11FrequencyBand(const Microsoft::Net::Wifi::Ieee80211FrequencyBand ieeeFrequencyBand) noexcept;

/**
 * @brief Convert the specified Dot11FrequencyBand to the equivalent IEEE 802.11 frequency band.
 *
 * @param dot11FrequencyBand The Dot11FrequencyBand to convert.
 * @return Microsoft::Net::Wifi::Ieee80211FrequencyBand
 */
Microsoft::Net::Wifi::Ieee80211FrequencyBand
FromDot11FrequencyBand(const Microsoft::Net::Wifi::Dot11FrequencyBand dot11FrequencyBand) noexcept;

/**
 * @brief Convert the specified IEEE 802.11 authentication algorithm to the equivalent Dot11AuthenticationAlgorithm.
 *
 * @param ieeeAuthenticationAlgorithm The IEEE 802.11 authentication algorithm to convert.
 * @return Microsoft::Net::Wifi::Dot11AuthenticationAlgorithm
 */
Microsoft::Net::Wifi::Dot11AuthenticationAlgorithm
ToDot11AuthenticationAlgorithm(const Microsoft::Net::Wifi::Ieee80211AuthenticationAlgorithm ieeeAuthenticationAlgorithm) noexcept;

/**
 * @brief Convert the specified Dot11AuthenticationAlgorithm to the equivalent IEEE 802.11 authentication algorithm.
 *
 * @param dot11AuthenticationAlgorithm The Dot11AuthenticationAlgorithm to convert.
 * @return Microsoft::Net::Wifi::Ieee80211AuthenticationAlgorithm
 */
Microsoft::Net::Wifi::Ieee80211AuthenticationAlgorithm
FromDot11AuthenticationAlgorithm(const Microsoft::Net::Wifi::Dot11AuthenticationAlgorithm dot11AuthenticationAlgorithm) noexcept;

/**
 * @brief Convert the specified IEEE 802.11 AKM suite algorithm to the equivalent Dot11CipherAlgorithm.
 *
 * @param ieeeAkmSuite The IEEE 802.11 AKM suite algorithm to convert.
 * @return Microsoft::Net::Wifi::Dot11AkmSuite
 */
Microsoft::Net::Wifi::Dot11AkmSuite
ToDot11AkmSuite(const Microsoft::Net::Wifi::Ieee80211AkmSuite ieeeAkmSuite) noexcept;

/**
 * @brief Convert the specified Dot11AkmSuite to the equivalent IEEE 802.11 AKM suite algorithm.
 *
 * @param dot11AkmSuite The Dot11AkmSuite to convert.
 * @return Microsoft::Net::Wifi::Ieee80211AkmSuite
 */
Microsoft::Net::Wifi::Ieee80211AkmSuite
FromDot11AkmSuite(const Microsoft::Net::Wifi::Dot11AkmSuite dot11AkmSuite) noexcept;

} // namespace Microsoft::Net::Wifi

#endif // IEEE_80211_DOT11_ADAPTERS_HXX
