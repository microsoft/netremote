
#include <algorithm>
#include <array>
#include <cerrno>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <format>
#include <functional>
#include <optional>
#include <ranges>
#include <span>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

#include <linux/genetlink.h>
#include <linux/netlink.h>
#include <linux/nl80211.h>
#include <magic_enum.hpp>
#include <microsoft/net/netlink/NetlinkMessage.hxx>
#include <microsoft/net/netlink/nl80211/Ieee80211Nl80211Adapters.hxx>
#include <microsoft/net/netlink/nl80211/Netlink80211.hxx>
#include <microsoft/net/netlink/nl80211/Netlink80211ProtocolState.hxx>
#include <microsoft/net/netlink/nl80211/Netlink80211Wiphy.hxx>
#include <microsoft/net/netlink/nl80211/Netlink80211WiphyBand.hxx>
#include <microsoft/net/wifi/Ieee80211.hxx>
#include <net/if.h>
#include <netlink/attr.h>
#include <netlink/errno.h>
#include <netlink/genl/genl.h>
#include <netlink/handlers.h>
#include <netlink/msg.h>
#include <netlink/netlink.h>
#include <netlink/socket.h>
#include <plog/Log.h>

using namespace Microsoft::Net::Netlink::Nl80211;

using Microsoft::Net::Wifi::Ieee80211AkmSuite;
using Microsoft::Net::Wifi::Ieee80211CipherSuite;

Nl80211Wiphy::Nl80211Wiphy(uint32_t index, std::string_view name, std::vector<uint32_t> akmSuites, std::vector<uint32_t> cipherSuites, std::unordered_map<nl80211_band, Nl80211WiphyBand> bands, std::vector<nl80211_iftype> supportedInterfaceTypes, std::vector<nl80211_wpa_versions> wpaVersions, bool supportsRoaming) noexcept :
    Index(index),
    Name(name),
    AkmSuites(std::move(akmSuites)),
    CipherSuites(std::move(cipherSuites)),
    Bands(std::move(bands)),
    SupportedInterfaceTypes(std::move(supportedInterfaceTypes)),
    WpaVersions(std::move(wpaVersions)),
    SupportsRoaming(supportsRoaming)
{
}

namespace detail
{
int
HandleNl80211GetWiphyResponse(struct nl_msg *nl80211Message, void *context) noexcept
{
    if (context == nullptr) {
        LOGE << "Received nl80211 get wiphy response with null context";
        return NL_SKIP;
    }

    // Extract the std::optional<Nl80211Wiphy> from the context.
    auto &nl80211WiphyResult = *static_cast<std::optional<Nl80211Wiphy> *>(context);

    // Attempt to parse the message.
    nl80211WiphyResult = Nl80211Wiphy::Parse(nl80211Message);
    if (!nl80211WiphyResult.has_value()) {
        LOGE << "Failed to parse nl80211 wiphy message";
        return NL_SKIP;
    }

    LOGD << std::format("Successfully parsed an nl80211 wiphy:\n{}", nl80211WiphyResult->ToString());

    return NL_OK;
}

using Microsoft::Net::Wifi::Nl80211WpaVersionToIeee80211SecurityProtocol;

/**
 * @brief Determine if the given WPA version is supported by the given cipher suites.
 *
 * @param wpaVersion The WPA version to check.
 * @param cipherSuites The cipher suites to check.
 * @return true If the cipher suites support the WPA version.
 * @return false It the cipher suites do not support the WPA version.
 */
bool
IsWpaVersionSupportedByCipherSuites(nl80211_wpa_versions wpaVersion, std::span<uint32_t> cipherSuites)
{
    constexpr auto toIeee80211CipherSuite = [](uint32_t nl80211CipherSuite) noexcept -> Ieee80211CipherSuite {
        return static_cast<Ieee80211CipherSuite>(nl80211CipherSuite);
    };

    const auto ieee80211SecurityProtocol = Nl80211WpaVersionToIeee80211SecurityProtocol(wpaVersion);
    const auto ieee80211CipherSuites = cipherSuites | std::views::transform(toIeee80211CipherSuite);
    const auto ieee80211CipherSuitesForWpa = WpaCipherSuites(ieee80211SecurityProtocol);
    const auto isWpaVersionSupported = std::ranges::find_first_of(ieee80211CipherSuites, ieee80211CipherSuitesForWpa) != std::ranges::end(ieee80211CipherSuites);

    return isWpaVersionSupported;
}

/**
 * @brief Determine if the given WPA version is supported by the given AKM suites.
 *
 * @param wpaVersion The WPA version to check.
 * @param akmSuites The AKM suites to check.
 * @return true If the AKM suites support the WPA version.
 * @return false If the AKM suites do not support the WPA version.
 */
bool
IsWpaVersionSupportedByAkmSuites(nl80211_wpa_versions wpaVersion, std::span<uint32_t> akmSuites)
{
    constexpr auto toIeee80211AkmSuite = [](uint32_t nl80211AkmSuite) noexcept -> Ieee80211AkmSuite {
        return static_cast<Ieee80211AkmSuite>(nl80211AkmSuite);
    };

    const auto ieee80211SecurityProtocol = Nl80211WpaVersionToIeee80211SecurityProtocol(wpaVersion);
    const auto ieee80211AkmSuites = akmSuites | std::views::transform(toIeee80211AkmSuite);
    const auto ieee80211AkmSuitesForWpa = WpaAkmSuites(ieee80211SecurityProtocol);
    const auto isWpaVersionSupported = std::ranges::find_first_of(ieee80211AkmSuites, ieee80211AkmSuitesForWpa) != std::ranges::end(ieee80211AkmSuites);

    return isWpaVersionSupported;
}

/**
 * @brief Determine if the given WPA version is supported by the given cipher suites and AKM suites.
 *
 * @param wpaVersion The WPA version to check.
 * @param cipherSuites The cipher suites to check.
 * @param akmSuites The AKM suites to check.
 * @return true If the cipher suites and AKM suites support the WPA version.
 * @return false If the cipher suites and AKM suites do not support the WPA version.
 */
bool
IsWpaVersionSupported(nl80211_wpa_versions wpaVersion, std::span<uint32_t> cipherSuites, std::span<uint32_t> akmSuites)
{
    return IsWpaVersionSupportedByCipherSuites(wpaVersion, cipherSuites) && IsWpaVersionSupportedByAkmSuites(wpaVersion, akmSuites);
}
} // namespace detail

/* static */
std::optional<Nl80211Wiphy>
Nl80211Wiphy::FromId(const std::function<void(NetlinkMessage &)> &addWiphyIdentifier)
{
    // Allocate a new nl80211 message for sending the dump request for all interfaces.
    auto nl80211MessageGetWiphy{ NetlinkMessage::Allocate() };
    if (nl80211MessageGetWiphy == nullptr) {
        LOGE << "Failed to allocate nl80211 message for wiphy request";
        return std::nullopt;
    }

    // Populate the genl message for the wiphy request.
    const int nl80211DriverId = Nl80211ProtocolState::Instance().DriverId;
    const auto *genlMessageGetInterfaces = genlmsg_put(nl80211MessageGetWiphy, NL_AUTO_PID, NL_AUTO_SEQ, nl80211DriverId, 0, 0, NL80211_CMD_GET_WIPHY, 0);
    if (genlMessageGetInterfaces == nullptr) {
        LOGE << "Failed to populate genl message for interface dump request";
        return std::nullopt;
    }

    // Add the identifier to the message so nl80211 knows what to lookup.
    addWiphyIdentifier(nl80211MessageGetWiphy);

    auto nl80211Socket{ CreateNl80211Socket() };
    std::optional<Nl80211Wiphy> nl80211Wiphy{};
    int ret = nl_socket_modify_cb(nl80211Socket, NL_CB_VALID, NL_CB_CUSTOM, detail::HandleNl80211GetWiphyResponse, &nl80211Wiphy);
    if (ret < 0) {
        LOGE << std::format("Failed to modify socket callback with error {} ({})", ret, nl_geterror(ret));
        return std::nullopt;
    }

    // Send the request.
    ret = nl_send_auto(nl80211Socket, nl80211MessageGetWiphy);
    if (ret < 0) {
        LOGE << std::format("Failed to send get wiphy request with error {} ({})", ret, nl_geterror(ret));
        return std::nullopt;
    }

    // Receive the response, which will invoke the configured callback.
    ret = nl_recvmsgs_default(nl80211Socket);
    if (ret < 0) {
        LOGE << std::format("Failed to receive get wiphy response with error {} ({})", ret, nl_geterror(ret));
        return std::nullopt;
    }

    return nl80211Wiphy;
}

/* static */
std::optional<Nl80211Wiphy>
Nl80211Wiphy::FromIndex(uint32_t wiphyIndex)
{
    return FromId([wiphyIndex](NetlinkMessage &nl80211MessageGetWiphy) {
        nla_put_u32(nl80211MessageGetWiphy, NL80211_ATTR_WIPHY, wiphyIndex);
    });
}

/* static */
std::optional<Nl80211Wiphy>
Nl80211Wiphy::FromInterfaceName(std::string_view interfaceName)
{
    // Look up the interface index from the interface name.
    auto interfaceIndex = if_nametoindex(std::data(interfaceName));
    if (interfaceIndex == 0) {
        const auto error = errno;
        LOGE << std::format("Failed to get interface index for interface name {} ({})", interfaceName, error);
        return std::nullopt;
    }

    return FromId([interfaceIndex](NetlinkMessage &nl80211MessageGetWiphy) {
        nla_put_u32(nl80211MessageGetWiphy, NL80211_ATTR_IFINDEX, interfaceIndex);
    });
}

/* static */
std::optional<Nl80211Wiphy>
Nl80211Wiphy::Parse(struct nl_msg *nl80211Message) noexcept
{
    // Ensure the message is valid.
    if (nl80211Message == nullptr) {
        LOGE << "Received null nl80211 message";
        return std::nullopt;
    }

    // Ensure the message has a valid genl header.
    auto *nl80211MessageHeader{ static_cast<struct nlmsghdr *>(nlmsg_hdr(nl80211Message)) };
    if (genlmsg_valid_hdr(nl80211MessageHeader, 1) < 0) {
        LOGE << "Received invalid nl80211 message header";
        return std::nullopt;
    }

    // Extract the nl80211 (genl) message header.
    const auto *genl80211MessageHeader{ static_cast<struct genlmsghdr *>(nlmsg_data(nl80211MessageHeader)) };

    // Parse the message.
    std::array<struct nlattr *, NL80211_ATTR_MAX + 1> wiphyAttributes{};
    int ret = nla_parse(std::data(wiphyAttributes), std::size(wiphyAttributes) - 1, genlmsg_attrdata(genl80211MessageHeader, 0), genlmsg_attrlen(genl80211MessageHeader, 0), nullptr);
    if (ret < 0) {
        LOGE << std::format("Failed to parse netlink message attributes with error {} ({})", ret, strerror(-ret)); // NOLINT(concurrency-mt-unsafe)
        return std::nullopt;
    }

    // Process top-level identifiers.
    if (wiphyAttributes[NL80211_ATTR_WIPHY] == nullptr || wiphyAttributes[NL80211_ATTR_WIPHY_NAME] == nullptr) {
        LOGE << "Received nl80211 message with missing wiphy index or name";
        return std::nullopt;
    }

    auto wiphyIndex = static_cast<uint32_t>(nla_get_u32(wiphyAttributes[NL80211_ATTR_WIPHY]));
    const auto *wiphyName = static_cast<const char *>(nla_data(wiphyAttributes[NL80211_ATTR_WIPHY_NAME]));

    // Process bands.
    auto *wiphyBands = wiphyAttributes[NL80211_ATTR_WIPHY_BANDS];
    std::unordered_map<nl80211_band, Nl80211WiphyBand> wiphyBandMap{};
    if (wiphyBands != nullptr) {
        int remainingBands = 0;
        struct nlattr *wiphyBand = nullptr;

        nla_for_each_nested(wiphyBand, wiphyBands, remainingBands)
        {
            auto nl80211BandType = static_cast<nl80211_band>(wiphyBand->nla_type);
            if (nl80211BandType == nl80211_band::NUM_NL80211_BANDS) {
                continue;
            }

            auto nl80211Band = Nl80211WiphyBand::Parse(wiphyBand);
            if (nl80211Band.has_value()) {
                wiphyBandMap.emplace(nl80211BandType, std::move(nl80211Band.value()));
            }
        }
    }

    // Process AKM suites.
    // Note: NL80211_ATTR_AKM_SUITES describes the AKMs supported by the PHY (wiphy) and is not specific to an interface.
    uint32_t *wiphyAkmSuites = nullptr;
    std::size_t wiphyNumAkmSuites = 0;
    std::vector<uint32_t> akmSuites{};
    if (wiphyAttributes[NL80211_ATTR_AKM_SUITES] != nullptr) {
        wiphyAkmSuites = static_cast<uint32_t *>(nla_data(wiphyAttributes[NL80211_ATTR_AKM_SUITES]));
        wiphyNumAkmSuites = static_cast<std::size_t>(nla_len(wiphyAttributes[NL80211_ATTR_AKM_SUITES])) / sizeof(*wiphyAkmSuites);
        akmSuites = std::vector<uint32_t>(wiphyAkmSuites, wiphyAkmSuites + wiphyNumAkmSuites); // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    } else {
        // Per nl80211 documentation, if this attribute is not present, userspace should assume all AKMs are supported.
        akmSuites = Microsoft::Net::Wifi::AllIeee80211Akms;
    }

    // Process cipher suites.
    std::size_t wiphyNumCipherSuites = 0;
    std::vector<uint32_t> cipherSuites{};
    if (wiphyAttributes[NL80211_ATTR_CIPHER_SUITES] != nullptr) {
        uint32_t *wiphyCipherSuites = nullptr;
        wiphyNumCipherSuites = static_cast<std::size_t>(nla_len(wiphyAttributes[NL80211_ATTR_CIPHER_SUITES])) / sizeof(*wiphyCipherSuites);
        wiphyCipherSuites = static_cast<uint32_t *>(nla_data(wiphyAttributes[NL80211_ATTR_CIPHER_SUITES]));
        cipherSuites = std::vector<uint32_t>(wiphyCipherSuites, wiphyCipherSuites + wiphyNumCipherSuites); // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    }

    // Process supported interface types.
    std::vector<nl80211_iftype> supportedInterfaceTypes{};
    if (wiphyAttributes[NL80211_ATTR_SUPPORTED_IFTYPES] != nullptr) {
        int remainingSupportedInterfaceTypes = 0;
        struct nlattr *supportedInterfaceType = nullptr;
        nla_for_each_nested(supportedInterfaceType, wiphyAttributes[NL80211_ATTR_SUPPORTED_IFTYPES], remainingSupportedInterfaceTypes)
        {
            auto interfaceType = static_cast<nl80211_iftype>(supportedInterfaceType->nla_type);
            supportedInterfaceTypes.emplace_back(interfaceType);
        }
    }

    // Process roaming support.
    auto wiphySupportsRoaming = wiphyAttributes[NL80211_ATTR_ROAM_SUPPORT] != nullptr;

    // Process security protocol support.
    // Note: The NL80211_ATTR_WPA_VERSIONS attribute technically describes this perfectly, but there is no way to obtain
    // the attribute value using a pure nl80211 attribute query. So, populate the values heuristically here based on
    // ciphers+akms supported that are exclusive to each version.
    std::vector<nl80211_wpa_versions> wpaVersions{};
    if (detail::IsWpaVersionSupported(nl80211_wpa_versions::NL80211_WPA_VERSION_1, cipherSuites, akmSuites)) {
        wpaVersions.emplace_back(nl80211_wpa_versions::NL80211_WPA_VERSION_1);
    }
    if (detail::IsWpaVersionSupported(nl80211_wpa_versions::NL80211_WPA_VERSION_2, cipherSuites, akmSuites)) {
        wpaVersions.emplace_back(nl80211_wpa_versions::NL80211_WPA_VERSION_2);
    }
    if (detail::IsWpaVersionSupported(nl80211_wpa_versions::NL80211_WPA_VERSION_3, cipherSuites, akmSuites)) {
        wpaVersions.emplace_back(nl80211_wpa_versions::NL80211_WPA_VERSION_3);
    }

    Nl80211Wiphy nl80211Wiphy{ wiphyIndex, wiphyName, std::move(akmSuites), std::move(cipherSuites), std::move(wiphyBandMap), std::move(supportedInterfaceTypes), std::move(wpaVersions), wiphySupportsRoaming };
    return nl80211Wiphy;
}

std::string
Nl80211Wiphy::ToString() const
{
    std::ostringstream ss;

    ss << std::format("Wiphy {} [{}]\n", Name, Index);
    ss << std::format(" Supports roaming: {}\n", SupportsRoaming);

    constexpr auto WpaVersionPrefixLength = std::size(std::string_view("NL80211_WPA_VERSION_"));
    ss << " WPA Versions:\n  ";
    for (const auto &wpaVersion : WpaVersions) {
        std::string_view wpaVersionName{ magic_enum::enum_name(wpaVersion) };
        wpaVersionName.remove_prefix(WpaVersionPrefixLength);
        ss << std::format("{} ", wpaVersionName);
    }

    ss << "\n Cipher Suites:\n  ";
    for (const auto &cipherSuite : CipherSuites) {
        ss << std::format("{} ", Nl80211CipherSuiteToString(cipherSuite));
    }

    constexpr auto IfTypePrefixLength = std::size(std::string_view("NL80211_IFTYPE_"));
    ss << "\n Supported Interface Types:\n";
    for (const auto &interfaceType : SupportedInterfaceTypes) {
        std::string_view interfaceTypeName{ magic_enum::enum_name(interfaceType) };
        interfaceTypeName.remove_prefix(IfTypePrefixLength);
        ss << std::format("  {}\n", interfaceTypeName);
    }

    constexpr auto BandPrefixLength = std::size(std::string_view("NL80211_BAND_"));
    ss << "\n Bands:\n";
    for (const auto &[band, wiphyBand] : Bands) {
        std::string_view bandName{ magic_enum::enum_name(band) };
        bandName.remove_prefix(BandPrefixLength);
        ss << std::format("  [Band {}]\n{}\n", bandName, wiphyBand.ToString());
    }

    return ss.str();
}
