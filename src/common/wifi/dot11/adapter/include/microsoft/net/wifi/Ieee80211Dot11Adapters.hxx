
#ifndef IEEE_80211_DOT11_ADAPTERS_HXX
#define IEEE_80211_DOT11_ADAPTERS_HXX

#include <unordered_map>
#include <vector>

#include <microsoft/net/remote/protocol/NetRemoteWifi.pb.h>
#include <microsoft/net/remote/protocol/WifiCore.pb.h>
#include <microsoft/net/wifi/AccessPointOperationStatus.hxx>
#include <microsoft/net/wifi/Ieee80211.hxx>
#include <microsoft/net/wifi/Ieee80211AccessPointCapabilities.hxx>

namespace Microsoft::Net::Wifi
{
/**
 * @brief Convert the specified WifiAccessPointOperationStatusCode to the equivalent AccessPointOperationStatus.
 *
 * @param accessPointOperationStatusCode The WifiAccessPointOperationStatusCode to convert.
 * @return Microsoft::Net::Remote::Wifi::WifiAccessPointOperationStatusCode
 */
Microsoft::Net::Remote::Wifi::WifiAccessPointOperationStatusCode
ToDot11AccessPointOperationStatusCode(Microsoft::Net::Wifi::AccessPointOperationStatusCode& accessPointOperationStatusCode) noexcept;

/**
 * @brief Convert the specified AccessPointOperationStatus to the equivalent WifiAccessPointOperationStatusCode.
 *
 * @param wifiAccessPointOperationStatusCode The AccessPointOperationStatus to convert.
 * @return Microsoft::Net::Wifi::AccessPointOperationStatusCode
 */
Microsoft::Net::Wifi::AccessPointOperationStatusCode
FromDot11AccessPointOperationStatusCode(Microsoft::Net::Remote::Wifi::WifiAccessPointOperationStatusCode wifiAccessPointOperationStatusCode) noexcept;

/**
 * @brief Convert the specified Ieee80211SecurityProtocol to the equivalent Dot11SecurityProtocol.
 *
 * @param ieee80211SecurityProtocol The IEEE 802.11 security protocol to convert.
 * @return Microsoft::Net::Wifi::Dot11SecurityProtocol
 */
Microsoft::Net::Wifi::Dot11SecurityProtocol
ToDot11SecurityProtocol(Microsoft::Net::Wifi::Ieee80211SecurityProtocol ieee80211SecurityProtocol) noexcept;

/**
 * @brief Convert the specified Dot11SecurityProtocol to the equivalent Ieee80211SecurityProtocol.
 *
 * @param dot11SecurityProtocol The Dot11SecurityProtocol to convert.
 * @return Microsoft::Net::Wifi::Ieee80211SecurityProtocol
 */
Microsoft::Net::Wifi::Ieee80211SecurityProtocol
FromDot11SecurityProtocol(Microsoft::Net::Wifi::Dot11SecurityProtocol dot11SecurityProtocol) noexcept;

/**
 * @brief Convert the specified Dot11PhyType to the equivalent IEEE 802.11 protocol.
 *
 * @param ieee80211PhyType The IEEE 802.11 PHY type to convert.
 * @return Microsoft::Net::Wifi::Dot11PhyType
 */
Microsoft::Net::Wifi::Dot11PhyType
ToDot11PhyType(Microsoft::Net::Wifi::Ieee80211PhyType ieee80211PhyType) noexcept;

/**
 * @brief Convert the specified Dot11PhyType to the equivalent IEEE 802.11 protocol.
 *
 * @param dot11PhyType The Dot11PhyType to convert.
 * @return Microsoft::Net::Wifi::Ieee80211PhyType
 */
Microsoft::Net::Wifi::Ieee80211PhyType
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
 * @brief Convert the specified IEEE 802.11 frequency bands to the equivalent Dot11FrequencyBands.
 *
 * @param ieee80211FrequencyBands The IEEE 802.11 frequency bands to convert.
 * @return std::vector<Microsoft::Net::Wifi::Dot11FrequencyBand>
 */
std::vector<Microsoft::Net::Wifi::Dot11FrequencyBand>
ToDot11FrequencyBands(const std::vector<Microsoft::Net::Wifi::Ieee80211FrequencyBand>& ieee80211FrequencyBands) noexcept;

/**
 * @brief Obtain a vector of Dot11FrequencyBands from the specified WifiAccessPointSetFrequencyBandsRequest.
 *
 * @param request The request to extract the Dot11FrequencyBands from.
 * @return std::vector<Microsoft::Net::Wifi::Dot11FrequencyBand>
 */
std::vector<Microsoft::Net::Wifi::Dot11FrequencyBand>
ToDot11FrequencyBands(const Microsoft::Net::Remote::Wifi::WifiAccessPointSetFrequencyBandsRequest& request) noexcept;

/**
 * @brief Obtain a vector of Dot11FrequencyBands from the specified Dot11AccessPointConfiguration.
 *
 * @param dot11AccessPointConfiguration The Dot11AccessPointConfiguration to extract the Dot11FrequencyBands from.
 * @return std::vector<Microsoft::Net::Wifi::Dot11FrequencyBand>
 */
std::vector<Microsoft::Net::Wifi::Dot11FrequencyBand>
ToDot11FrequencyBands(const Microsoft::Net::Wifi::Dot11AccessPointConfiguration& dot11AccessPointConfiguration) noexcept;

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
 * @brief Convert the specified IEEE 802.11 authentication algorithms to the equivalent Dot11AuthenticationAlgorithms.
 *
 * @param ieee80211AuthenticationAlgorithms The IEEE 802.11 authentication algorithms to convert.
 * @return std::vector<Microsoft::Net::Wifi::Dot11AuthenticationAlgorithm>
 */
std::vector<Microsoft::Net::Wifi::Dot11AuthenticationAlgorithm>
ToDot11AuthenticationAlgorithms(const std::vector<Microsoft::Net::Wifi::Ieee80211AuthenticationAlgorithm>& ieee80211AuthenticationAlgorithms) noexcept;

/**
 * @brief Obtain a vector of Dot11AuthenticationAlgorithms from the specified Dot11AccessPointConfiguration.
 *
 * @param dot11AccessPointConfiguration The Dot11AccessPointConfiguration to extract the Dot11AuthenticationAlgorithms from.
 * @return std::vector<Microsoft::Net::Wifi::Dot11AuthenticationAlgorithm>
 */
std::vector<Microsoft::Net::Wifi::Dot11AuthenticationAlgorithm>
ToDot11AuthenticationAlgorithms(const Microsoft::Net::Wifi::Dot11AccessPointConfiguration& dot11AccessPointConfiguration) noexcept;

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
 * @brief Convert the specified repeated field of Dot11CipherSuiteConfigurations to the equivalent map of Dot11SecurityProtocol to Dot11CipherSuite.
 *
 * @param dot11CipherSuiteConfigurations The repeated field of Dot11CipherSuiteConfigurations to convert.
 * @return std::unordered_map<Microsoft::Net::Wifi::Dot11SecurityProtocol, std::vector<Microsoft::Net::Wifi::Dot11CipherSuite>>
 */
std::unordered_map<Microsoft::Net::Wifi::Dot11SecurityProtocol, std::vector<Microsoft::Net::Wifi::Dot11CipherSuite>>
ToDot11CipherSuiteConfigurations(const google::protobuf::RepeatedPtrField<Microsoft::Net::Wifi::Dot11CipherSuiteConfiguration>& dot11CipherSuiteConfigurations) noexcept;

/**
 * @brief Convert the specified map of Ieee80211SecurityProtocol to Ieee80211CipherSuite to the equivalent vector of Dot11CipherSuiteConfiguratios.
 *
 * @param ieee80211CipherSuiteConfigurations The map of Ieee80211SecurityProtocol to Ieee80211CipherSuite to convert.
 * @return std::vector<Microsoft::Net::Wifi::Dot11CipherSuiteConfiguration>
 */
std::vector<Microsoft::Net::Wifi::Dot11CipherSuiteConfiguration>
ToDot11CipherSuiteConfigurations(const std::unordered_map<Ieee80211SecurityProtocol, std::vector<Ieee80211CipherSuite>>& ieee80211CipherSuiteConfigurations) noexcept;

/**
 * @brief Convert the specified map of Dot11SecurityProtocol to Dot11CipherSuite to the equivalent map of IEEE 802.11 security protocol to cipher suite.
 *
 * @param dot11CipherSuiteConfigurations The map of Dot11SecurityProtocol to Dot11CipherSuite to convert.
 * @return std::unordered_map<Microsoft::Net::Wifi::Ieee80211SecurityProtocol, std::vector<Microsoft::Net::Wifi::Ieee80211CipherSuite>>
 */
std::unordered_map<Microsoft::Net::Wifi::Ieee80211SecurityProtocol, std::vector<Microsoft::Net::Wifi::Ieee80211CipherSuite>>
FromDot11CipherSuiteConfigurations(const std::unordered_map<Microsoft::Net::Wifi::Dot11SecurityProtocol, std::vector<Microsoft::Net::Wifi::Dot11CipherSuite>>& dot11CipherSuiteConfigurations) noexcept;

/**
 * @brief Convert the specified IEEE 802.11 access point capabilities to the equivalent Dot11AccessPointCapabilities.
 *
 * @param ieee80211AccessPointCapabilities The IEEE 802.11 access point capabilities to convert.
 * @return Microsoft::Net::Wifi::Dot11AccessPointCapabilities
 */
Microsoft::Net::Wifi::Dot11AccessPointCapabilities
ToDot11AccessPointCapabilities(const Microsoft::Net::Wifi::Ieee80211AccessPointCapabilities& ieee80211AccessPointCapabilities) noexcept;

} // namespace Microsoft::Net::Wifi

#endif // IEEE_80211_DOT11_ADAPTERS_HXX
