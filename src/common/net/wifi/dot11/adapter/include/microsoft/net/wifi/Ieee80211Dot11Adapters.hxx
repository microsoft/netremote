
#ifndef IEEE_80211_DOT11_ADAPTERS_HXX
#define IEEE_80211_DOT11_ADAPTERS_HXX

#include <unordered_map>
#include <vector>

#include <microsoft/net/remote/protocol/NetRemoteWifi.pb.h>
#include <microsoft/net/remote/protocol/Network8021x.pb.h>
#include <microsoft/net/remote/protocol/WifiCore.pb.h>
#include <microsoft/net/wifi/AccessPointOperationStatus.hxx>
#include <microsoft/net/wifi/IAccessPoint.hxx>
#include <microsoft/net/wifi/Ieee80211.hxx>
#include <microsoft/net/wifi/Ieee80211AccessPointCapabilities.hxx>
#include <microsoft/net/wifi/Ieee80211Authentication.hxx>

namespace Microsoft::Net::Wifi
{
/**
 * @brief Convert the specified WifiAccessPointOperationStatusCode to the equivalent AccessPointOperationStatus.
 *
 * @param accessPointOperationStatusCode The WifiAccessPointOperationStatusCode to convert.
 * @return Microsoft::Net::Remote::Wifi::WifiAccessPointOperationStatusCode
 */
Microsoft::Net::Remote::Wifi::WifiAccessPointOperationStatusCode
ToDot11AccessPointOperationStatusCode(AccessPointOperationStatusCode& accessPointOperationStatusCode) noexcept;

/**
 * @brief Convert the specified AccessPointOperationStatus to the equivalent WifiAccessPointOperationStatusCode.
 *
 * @param wifiAccessPointOperationStatusCode The AccessPointOperationStatus to convert.
 * @return AccessPointOperationStatusCode
 */
AccessPointOperationStatusCode
FromDot11AccessPointOperationStatusCode(Microsoft::Net::Remote::Wifi::WifiAccessPointOperationStatusCode wifiAccessPointOperationStatusCode) noexcept;

/**
 * @brief Convert the specified Ieee80211SecurityProtocol to the equivalent Dot11SecurityProtocol.
 *
 * @param ieee80211SecurityProtocol The IEEE 802.11 security protocol to convert.
 * @return Dot11SecurityProtocol
 */
Dot11SecurityProtocol
ToDot11SecurityProtocol(Ieee80211SecurityProtocol ieee80211SecurityProtocol) noexcept;

/**
 * @brief Convert the specified Dot11SecurityProtocol to the equivalent Ieee80211SecurityProtocol.
 *
 * @param dot11SecurityProtocol The Dot11SecurityProtocol to convert.
 * @return Ieee80211SecurityProtocol
 */
Ieee80211SecurityProtocol
FromDot11SecurityProtocol(Dot11SecurityProtocol dot11SecurityProtocol) noexcept;

/**
 * @brief Convert the specified Dot11PhyType to the equivalent IEEE 802.11 protocol.
 *
 * @param ieee80211PhyType The IEEE 802.11 PHY type to convert.
 * @return Dot11PhyType
 */
Dot11PhyType
ToDot11PhyType(Ieee80211PhyType ieee80211PhyType) noexcept;

/**
 * @brief Convert the specified Dot11PhyType to the equivalent IEEE 802.11 protocol.
 *
 * @param dot11PhyType The Dot11PhyType to convert.
 * @return Ieee80211PhyType
 */
Ieee80211PhyType
FromDot11PhyType(Dot11PhyType dot11PhyType) noexcept;

/**
 * @brief Convert the specified Dot11FrequencyBand to the equivalent IEEE 802.11 frequency band.
 *
 * @param ieee80211FrequencyBand The IEEE 802.11 frequency band to convert.
 * @return Dot11FrequencyBand
 */
Dot11FrequencyBand
ToDot11FrequencyBand(Ieee80211FrequencyBand ieee80211FrequencyBand) noexcept;

/**
 * @brief Convert the specified IEEE 802.11 frequency bands to the equivalent Dot11FrequencyBands.
 *
 * @param ieee80211FrequencyBands The IEEE 802.11 frequency bands to convert.
 * @return std::vector<Dot11FrequencyBand>
 */
std::vector<Dot11FrequencyBand>
ToDot11FrequencyBands(const std::vector<Ieee80211FrequencyBand>& ieee80211FrequencyBands) noexcept;

/**
 * @brief Obtain a vector of Dot11FrequencyBands from the specified WifiAccessPointSetFrequencyBandsRequest.
 *
 * @param request The request to extract the Dot11FrequencyBands from.
 * @return std::vector<Dot11FrequencyBand>
 */
std::vector<Dot11FrequencyBand>
ToDot11FrequencyBands(const Microsoft::Net::Remote::Wifi::WifiAccessPointSetFrequencyBandsRequest& request) noexcept;

/**
 * @brief Obtain a vector of Dot11FrequencyBands from the specified Dot11AccessPointConfiguration.
 *
 * @param dot11AccessPointConfiguration The Dot11AccessPointConfiguration to extract the Dot11FrequencyBands from.
 * @return std::vector<Dot11FrequencyBand>
 */
std::vector<Dot11FrequencyBand>
ToDot11FrequencyBands(const Dot11AccessPointConfiguration& dot11AccessPointConfiguration) noexcept;

/**
 * @brief Convert the specified Dot11FrequencyBand to the equivalent IEEE 802.11 frequency band.
 *
 * @param dot11FrequencyBand The Dot11FrequencyBand to convert.
 * @return Ieee80211FrequencyBand
 */
Ieee80211FrequencyBand
FromDot11FrequencyBand(Dot11FrequencyBand dot11FrequencyBand) noexcept;

/**
 * @brief Obtain the equivalent IEEE 802.11 frequency band from the Dot11FrequencyBands in the
 * WifiAccessPointSetFrequencyBandsRequest.
 *
 * @param request
 * @return std::vector<Ieee80211FrequencyBand>
 */
std::vector<Ieee80211FrequencyBand>
FromDot11SetFrequencyBandsRequest(const Microsoft::Net::Remote::Wifi::WifiAccessPointSetFrequencyBandsRequest& request);

/**
 * @brief Convert the specified IEEE 802.11 authentication algorithm to the equivalent Dot11AuthenticationAlgorithm.
 *
 * @param ieee80211AuthenticationAlgorithm The IEEE 802.11 authentication algorithm to convert.
 * @return Dot11AuthenticationAlgorithm
 */
Dot11AuthenticationAlgorithm
ToDot11AuthenticationAlgorithm(Ieee80211AuthenticationAlgorithm ieee80211AuthenticationAlgorithm) noexcept;

/**
 * @brief Convert the specified IEEE 802.11 authentication algorithms to the equivalent Dot11AuthenticationAlgorithms.
 *
 * @param ieee80211AuthenticationAlgorithms The IEEE 802.11 authentication algorithms to convert.
 * @return std::vector<Dot11AuthenticationAlgorithm>
 */
std::vector<Dot11AuthenticationAlgorithm>
ToDot11AuthenticationAlgorithms(const std::vector<Ieee80211AuthenticationAlgorithm>& ieee80211AuthenticationAlgorithms) noexcept;

/**
 * @brief Obtain a vector of Dot11AuthenticationAlgorithms from the specified Dot11AccessPointConfiguration.
 *
 * @param dot11AccessPointConfiguration The Dot11AccessPointConfiguration to extract the Dot11AuthenticationAlgorithms from.
 * @return std::vector<Dot11AuthenticationAlgorithm>
 */
std::vector<Dot11AuthenticationAlgorithm>
ToDot11AuthenticationAlgorithms(const Dot11AccessPointConfiguration& dot11AccessPointConfiguration) noexcept;

/**
 * @brief Convert the specified Dot11AuthenticationAlgorithm to the equivalent IEEE 802.11 authentication algorithm.
 *
 * @param dot11AuthenticationAlgorithm The Dot11AuthenticationAlgorithm to convert.
 * @return Ieee80211AuthenticationAlgorithm
 */
Ieee80211AuthenticationAlgorithm
FromDot11AuthenticationAlgorithm(Dot11AuthenticationAlgorithm dot11AuthenticationAlgorithm) noexcept;

/**
 * @brief Convert the specified IEEE 802.11 AKM suite algorithm to the equivalent Dot11CipherAlgorithm.
 *
 * @param ieee80211AkmSuite The IEEE 802.11 AKM suite algorithm to convert.
 * @return Dot11AkmSuite
 */
Dot11AkmSuite
ToDot11AkmSuite(Ieee80211AkmSuite ieee80211AkmSuite) noexcept;

/**
 * @brief Obtain a vector of Dot11AkmSuites from the specified Dot11AccessPointConfiguration.
 *
 * @param dot11AccessPointConfiguration The Dot11AccessPointConfiguration to extract the Dot11AkmSuites from.
 * @return std::vector<Dot11AkmSuite>
 */
std::vector<Dot11AkmSuite>
ToDot11AkmSuites(const Dot11AccessPointConfiguration& dot11AccessPointConfiguration) noexcept;

/**
 * @brief Convert the specified IEEE 802.11 AKM suite algorithms to the equivalent Dot11AkmSuites.
 *
 * @param ieee80211AkmSuites The IEEE 802.11 AKM suite algorithms to convert.
 * @return std::vector<Dot11AkmSuite>
 */
std::vector<Dot11AkmSuite>
ToDot11AkmSuites(const std::vector<Ieee80211AkmSuite>& ieee80211AkmSuites) noexcept;

/**
 * @brief Convert the specified Dot11AkmSuite to the equivalent IEEE 802.11 AKM suite algorithm.
 *
 * @param dot11AkmSuite The Dot11AkmSuite to convert.
 * @return Ieee80211AkmSuite
 */
Ieee80211AkmSuite
FromDot11AkmSuite(Dot11AkmSuite dot11AkmSuite) noexcept;

/**
 * @brief Convert the specified IEEE 802.11 cipher suite algorithm to the equivalent Dot11CipherSuite.
 *
 * @param ieee80211CipherSuite The IEEE 802.11 cipher suite algorithm to convert.
 * @return Dot11CipherSuite
 */
Dot11CipherSuite
ToDot11CipherSuite(Ieee80211CipherSuite ieee80211CipherSuite) noexcept;

/**
 * @brief Convert the specified Dot11CipherSuite to the equivalent IEEE 802.11 cipher suite algorithm.
 *
 * @param dot11CipherSuite The Dot11CipherSuite to convert.
 * @return Ieee80211CipherSuite
 */
Ieee80211CipherSuite
FromDot11CipherSuite(Dot11CipherSuite dot11CipherSuite) noexcept;

/**
 * @brief Convert the specified repeated field of Dot11CipherSuiteConfigurations to the equivalent map of Dot11SecurityProtocol to Dot11CipherSuite.
 *
 * @param dot11CipherSuiteConfigurations The repeated field of Dot11CipherSuiteConfigurations to convert.
 * @return std::unordered_map<Dot11SecurityProtocol, std::vector<Dot11CipherSuite>>
 */
std::unordered_map<Dot11SecurityProtocol, std::vector<Dot11CipherSuite>>
ToDot11CipherSuiteConfigurations(const google::protobuf::RepeatedPtrField<Dot11CipherSuiteConfiguration>& dot11CipherSuiteConfigurations) noexcept;

/**
 * @brief Convert the specified map of Ieee80211SecurityProtocol to Ieee80211CipherSuite to the equivalent vector of Dot11CipherSuiteConfiguratios.
 *
 * @param ieee80211CipherSuiteConfigurations The map of Ieee80211SecurityProtocol to Ieee80211CipherSuite to convert.
 * @return std::vector<Dot11CipherSuiteConfiguration>
 */
std::vector<Dot11CipherSuiteConfiguration>
ToDot11CipherSuiteConfigurations(const std::unordered_map<Ieee80211SecurityProtocol, std::vector<Ieee80211CipherSuite>>& ieee80211CipherSuiteConfigurations) noexcept;

/**
 * @brief Convert the specified map of Dot11SecurityProtocol to Dot11CipherSuite to the equivalent map of IEEE 802.11 security protocol to cipher suite.
 *
 * @param dot11CipherSuiteConfigurations The map of Dot11SecurityProtocol to Dot11CipherSuite to convert.
 * @return std::unordered_map<Ieee80211SecurityProtocol, std::vector<Ieee80211CipherSuite>>
 */
std::unordered_map<Ieee80211SecurityProtocol, std::vector<Ieee80211CipherSuite>>
FromDot11CipherSuiteConfigurations(const std::unordered_map<Dot11SecurityProtocol, std::vector<Dot11CipherSuite>>& dot11CipherSuiteConfigurations) noexcept;

/**
 * @brief Convert the specified IEEE 802.11 access point capabilities to the equivalent Dot11AccessPointCapabilities.
 *
 * @param ieee80211AccessPointCapabilities The IEEE 802.11 access point capabilities to convert.
 * @return Dot11AccessPointCapabilities
 */
Dot11AccessPointCapabilities
ToDot11AccessPointCapabilities(const Ieee80211AccessPointCapabilities& ieee80211AccessPointCapabilities) noexcept;

/**
 * @brief Convert the specified Dot11MacAddress to the equivalent IEEE 802.11 mac address.
 *
 * @param dot11MacAddress The Dot11MacAddress to convert.
 * @return Ieee80211MacAddress
 */
Ieee80211MacAddress
FromDot11MacAddress(const Dot11MacAddress& dot11MacAddress) noexcept;

/**
 * @brief Convert the specified IEEE 802.11 mac address to the equivalent Dot11MacAddress.
 *
 * @param ieee80211MacAddress The IEEE 802.11 mac address to convert.
 * @return Dot11MacAddress
 */
Dot11MacAddress
ToDot11MacAddress(const Ieee80211MacAddress& ieee80211MacAddress) noexcept;

/**
 * @brief Convert the specified Dot11RsnaPsk to the equivalent IEEE 802.11 shared key.
 *
 * @param Dot11RsnaPsk The Dot11RsnaPsk to convert.
 * @return Ieee80211RsnaPsk
 */
Ieee80211RsnaPsk
FromDot11RsnaPsk(const Dot11RsnaPsk& Dot11RsnaPsk) noexcept;

/**
 * @brief Convert the specified IEEE 802.11 shared key to the equivalent Dot11RsnaPsk.
 *
 * @param ieee80211RnsaPsk The IEEE 802.11 shared key to convert.
 * @return Dot11RsnaPsk
 */
Dot11RsnaPsk
ToDot11RsnaPsk(const Ieee80211RsnaPsk& ieee80211RnsaPsk) noexcept;

/**
 * @brief Convert the specified Dot11RsnaPassword to the equivalent IEEE 802.11 RSNA password.
 *
 * @param dot11RsnaPassword The Dot11RsnaPassword to convert.
 * @return Ieee80211RsnaPassword
 */
Ieee80211RsnaPassword
FromDot11RsnaPassword(const Dot11RsnaPassword& dot11RsnaPassword) noexcept;

/**
 * @brief Convert the specified IEEE 802.11 RSNA password to the equivalent Dot11RsnaPassword.
 *
 * @param ieee80211RsnaPassword The IEEE 802.11 RSNA password to convert.
 * @return Dot11RsnaPassword
 */
Dot11RsnaPassword
ToDot11RsnaPassword(const Ieee80211RsnaPassword& ieee80211RsnaPassword) noexcept;

/**
 * @brief Convert the specified Dot11AuthenticationDataPsk to the equivalent IEEE 802.11 authentication data.
 *
 * @param dot11AuthenticationDataPsk The Dot11AuthenticationDataPsk to convert.
 * @return Ieee80211AuthenticationDataPsk
 */
Ieee80211AuthenticationDataPsk
FromDot11AuthenticationDataPsk(const Dot11AuthenticationDataPsk& dot11AuthenticationDataPsk) noexcept;

/**
 * @brief Convert the specified IEEE 802.11 authentication data to the equivalent Dot11AuthenticationDataPsk.
 *
 * @param ieee80211AuthenticationDataPsk The IEEE 802.11 authentication data to convert.
 * @return Dot11AuthenticationDataPsk
 */
Dot11AuthenticationDataPsk
ToDot11AuthenticationDataPsk(const Ieee80211AuthenticationDataPsk& ieee80211AuthenticationDataPsk) noexcept;

/**
 * @brief Convert the specified Dot11AuthenticationDataSae to the equivalent IEEE 802.11 authentication data.
 *
 * @param dot11AuthenticationDataSae The Dot11AuthenticationDataSae to convert.
 * @return Ieee80211AuthenticationDataSae
 */
Ieee80211AuthenticationDataSae
FromDot11AuthenticationDataSae(const Dot11AuthenticationDataSae& dot11AuthenticationDataSae) noexcept;

/**
 * @brief Convert the specified IEEE 802.11 authentication data to the equivalent Dot11AuthenticationDataSae.
 *
 * @param ieee80211AuthenticationDataSae The IEEE 802.11 authentication data to convert.
 * @return Dot11AuthenticationDataSae
 */
Dot11AuthenticationDataSae
ToDot11AuthenticationDataSae(const Ieee80211AuthenticationDataSae& ieee80211AuthenticationDataSae) noexcept;

/**
 * @brief Convert the specified Dot11AuthenticationData to the equivalent IEEE 802.11 authentication data.
 *
 * @param dot11AuthenticationData The Dot11AuthenticationData to convert.
 * @return Ieee80211AuthenticationData
 */
Ieee80211AuthenticationData
FromDot11AuthenticationData(const Dot11AuthenticationData& dot11AuthenticationData) noexcept;

/**
 * @brief Convert the specified IEEE 802.11 authentication data to the equivalent Dot11AuthenticationData.
 *
 * @param ieee80211AuthenticationData The IEEE 802.11 authentication data to convert.
 * @return Dot11AuthenticationData
 */
Dot11AuthenticationData
ToDot11AuthenticationData(const Ieee80211AuthenticationData& ieee80211AuthenticationData) noexcept;

/**
 * @brief Convert the specified Dot11AuthenticationDot1x to the equivalent IEEE 802.1x authentication.
 *
 * @param dot11AuthenticationDot1x The Dot11AuthenticationDot1x to convert.
 * @return Ieee80211Authentication8021x
 */
Ieee80211Authentication8021x
FromDot11AuthenticationDot1x(const Dot11AuthenticationDot1x& dot11AuthenticationDot1x) noexcept;

/**
 * @brief Convert the specified IEEE 802.1x authentication to the equivalent Dot11AuthenticationDot1x.
 *
 * @param ieee8021xAuthentication The IEEE 802.1x authentication to convert.
 * @return Dot11AuthenticationDot1x
 */
Dot11AuthenticationDot1x
ToDot11AuthenticationDot1x(const Ieee80211Authentication8021x& ieee8021xAuthentication) noexcept;

/**
 * @brief Convert the specified Dot11AccessPointAttributes to the equivalent neutral type access point attributes.
 *
 * @param dot11AccessPointConfiguration The Dot11AccessPointAttributes to convert.
 * @return AccessPointAttributes
 */
AccessPointAttributes
FromDot11AccessPointAttributes(const Dot11AccessPointAttributes& dot11AccessPointConfiguration) noexcept;

/**
 * @brief Convert the specified neutral type access point attributes to the equivalent Dot11AccessPointAttributes.
 *
 * @param accessPointAttributes The IEEE 802.11 access point configuration to convert.
 * @return Dot11AccessPointAttributes
 */
Dot11AccessPointAttributes
ToDot11AccessPointAttributes(const AccessPointAttributes& accessPointAttributes) noexcept;

} // namespace Microsoft::Net::Wifi

#endif // IEEE_80211_DOT11_ADAPTERS_HXX
