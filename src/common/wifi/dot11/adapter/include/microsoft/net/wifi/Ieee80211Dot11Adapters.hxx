
#ifndef IEEE_80211_DOT11_ADAPTERS_HXX
#define IEEE_80211_DOT11_ADAPTERS_HXX

#include <vector>

#include <microsoft/net/remote/protocol/WifiCore.pb.h>
#include <microsoft/net/wifi/Ieee80211.hxx>
#include <microsoft/net/wifi/Ieee80211AccessPointCapabilities.hxx>

namespace Microsoft::Net::Wifi
{
/**
 * @brief Convert the specified Dot11PhyType to the equivalent IEEE 802.11 protocol.
 *
 * @param ieee80211Protocol The IEEE 802.11 protocol to convert.
 * @return Microsoft::Net::Wifi::Dot11PhyType
 */
Microsoft::Net::Wifi::Dot11PhyType
ToDot11PhyType(Microsoft::Net::Wifi::Ieee80211Protocol ieee80211Protocol) noexcept;

/**
 * @brief Convert the specified Dot11PhyType to the equivalent IEEE 802.11 protocol.
 *
 * @param dot11PhyType The Dot11PhyType to convert.
 * @return Microsoft::Net::Wifi::Ieee80211Protocol
 */
Microsoft::Net::Wifi::Ieee80211Protocol
FromDot11PhyType(Microsoft::Net::Wifi::Dot11PhyType dot11PhyType) noexcept;

/**
 * @brief Convert the specified Dot11FrequencyBand to the equivalent IEEE 802.11 frequency band.
 *
 * @param ieee80211FrequencyBand The IEEE 802.11 frequency band to convert.
 * @return Microsoft::Net::Wifi::Dot11FrequencyBand
 */
Microsoft::Net::Wifi::Dot11FrequencyBand
ToDot11FrequencyBand(Microsoft::Net::Wifi::Ieee80211FrequencyBand ieee80211FrequencyBand) noexcept;

/**
 * @brief Convert the specified Dot11FrequencyBand to the equivalent IEEE 802.11 frequency band.
 *
 * @param dot11FrequencyBand The Dot11FrequencyBand to convert.
 * @return Microsoft::Net::Wifi::Ieee80211FrequencyBand
 */
Microsoft::Net::Wifi::Ieee80211FrequencyBand
FromDot11FrequencyBand(Microsoft::Net::Wifi::Dot11FrequencyBand dot11FrequencyBand) noexcept;

/**
 * @brief Obtain the equivalent IEEE 802.11 frequency band from the Dot11FrequencyBands in the
 * WifiAccessPointSetFrequencyBandsRequest.
 *
 * @param request
 * @return std::vector<Microsoft::Net::Wifi::Ieee80211FrequencyBand>
 */
std::vector<Microsoft::Net::Wifi::Ieee80211FrequencyBand>
FromDot11SetFrequencyBandsRequest(const Microsoft::Net::Remote::Wifi::WifiAccessPointSetFrequencyBandsRequest& request);

/**
 * @brief Convert the specified IEEE 802.11 authentication algorithm to the equivalent Dot11AuthenticationAlgorithm.
 *
 * @param ieee80211AuthenticationAlgorithm The IEEE 802.11 authentication algorithm to convert.
 * @return Microsoft::Net::Wifi::Dot11AuthenticationAlgorithm
 */
Microsoft::Net::Wifi::Dot11AuthenticationAlgorithm
ToDot11AuthenticationAlgorithm(Microsoft::Net::Wifi::Ieee80211AuthenticationAlgorithm ieee80211AuthenticationAlgorithm) noexcept;

/**
 * @brief Convert the specified Dot11AuthenticationAlgorithm to the equivalent IEEE 802.11 authentication algorithm.
 *
 * @param dot11AuthenticationAlgorithm The Dot11AuthenticationAlgorithm to convert.
 * @return Microsoft::Net::Wifi::Ieee80211AuthenticationAlgorithm
 */
Microsoft::Net::Wifi::Ieee80211AuthenticationAlgorithm
FromDot11AuthenticationAlgorithm(Microsoft::Net::Wifi::Dot11AuthenticationAlgorithm dot11AuthenticationAlgorithm) noexcept;

/**
 * @brief Convert the specified IEEE 802.11 AKM suite algorithm to the equivalent Dot11CipherAlgorithm.
 *
 * @param ieee80211AkmSuite The IEEE 802.11 AKM suite algorithm to convert.
 * @return Microsoft::Net::Wifi::Dot11AkmSuite
 */
Microsoft::Net::Wifi::Dot11AkmSuite
ToDot11AkmSuite(Microsoft::Net::Wifi::Ieee80211AkmSuite ieee80211AkmSuite) noexcept;

/**
 * @brief Convert the specified Dot11AkmSuite to the equivalent IEEE 802.11 AKM suite algorithm.
 *
 * @param dot11AkmSuite The Dot11AkmSuite to convert.
 * @return Microsoft::Net::Wifi::Ieee80211AkmSuite
 */
Microsoft::Net::Wifi::Ieee80211AkmSuite
FromDot11AkmSuite(Microsoft::Net::Wifi::Dot11AkmSuite dot11AkmSuite) noexcept;

/**
 * @brief Convert the specified IEEE 802.11 cipher suite algorithm to the equivalent Dot11CipherSuite.
 *
 * @param ieee80211CipherSuite The IEEE 802.11 cipher suite algorithm to convert.
 * @return Microsoft::Net::Wifi::Dot11CipherSuite
 */
Microsoft::Net::Wifi::Dot11CipherSuite
ToDot11CipherSuite(Microsoft::Net::Wifi::Ieee80211CipherSuite ieee80211CipherSuite) noexcept;

/**
 * @brief Convert the specified Dot11CipherSuite to the equivalent IEEE 802.11 cipher suite algorithm.
 *
 * @param dot11CipherSuite The Dot11CipherSuite to convert.
 * @return Microsoft::Net::Wifi::Ieee80211CipherSuite
 */
Microsoft::Net::Wifi::Ieee80211CipherSuite
FromDot11CipherSuite(Microsoft::Net::Wifi::Dot11CipherSuite dot11CipherSuite) noexcept;

/**
 * @brief Convert the specified IEEE 802.11 access point capabilities to the equivalent Dot11AccessPointCapabilities.
 *
 * @param ieee80211AccessPointCapabilities The IEEE 802.11 access point capabilities to convert.
 * @return Microsoft::Net::Wifi::Dot11AccessPointCapabilities
 */
Microsoft::Net::Wifi::Dot11AccessPointCapabilities
ToDot11AccessPointCapabilities(const Microsoft::Net::Wifi::Ieee80211AccessPointCapabilities& ieee80211AccessPointCapabilities) noexcept;

/**
 * @brief Convert the specified Dot11AccessPointCapabilities to the equivalent IEEE 802.11 access point capabilities.
 *
 * @param dot11AccessPointCapabilities The Dot11AccessPointCapabilities to convert.
 * @return Microsoft::Net::Wifi::Ieee80211AccessPointCapabilities
 */
Microsoft::Net::Wifi::Ieee80211AccessPointCapabilities
FromDot11AccessPointCapabilities(const Microsoft::Net::Wifi::Dot11AccessPointCapabilities& dot11AccessPointCapabilities) /* noexcept */;

} // namespace Microsoft::Net::Wifi

#endif // IEEE_80211_DOT11_ADAPTERS_HXX
