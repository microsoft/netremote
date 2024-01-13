
#include <array>
#include <format>
#include <optional>
#include <sstream>

#include <magic_enum.hpp>
#include <microsoft/net/netlink/NetlinkMessage.hxx>
#include <microsoft/net/netlink/NetlinkSocket.hxx>
#include <microsoft/net/netlink/nl80211/Netlink80211.hxx>
#include <microsoft/net/netlink/nl80211/Netlink80211ProtocolState.hxx>
#include <microsoft/net/netlink/nl80211/Netlink80211Wiphy.hxx>
#include <netlink/attr.h>
#include <netlink/genl/genl.h>
#include <plog/Log.h>

using namespace Microsoft::Net::Netlink::Nl80211;

Nl80211WiphyBand::Nl80211WiphyBand(bool supportsRoaming) noexcept :
    SupportsRoaming(supportsRoaming)
{
}

std::string
Nl80211WiphyBand::ToString() const
{
    return std::format("Supports Roaming {}", SupportsRoaming);
}

Nl80211Wiphy::Nl80211Wiphy(uint32_t index, std::string_view name, std::vector<uint32_t> cipherSuites, std::unordered_map<nl80211_band, Nl80211WiphyBand> bands) noexcept :
    Index(index),
    Name(name),
    CipherSuites(std::move(cipherSuites)),
    Bands(std::move(bands))
{
}

std::string
Nl80211Wiphy::ToString() const
{
    std::ostringstream ss;
    ss << std::format("[{}] {}\n", Index, Name);

    ss << " Cipher Suites:\n  ";
    for (const auto &cipherSuite : CipherSuites) {
        ss << std::format("{:x} ", cipherSuite);
    }

    ss << "\n Bands:\n";
    for (const auto &[band, wiphyBand] : Bands) {
        ss << std::format("  {}: {}\n", magic_enum::enum_name(band), wiphyBand.ToString());
    }

    return ss.str();
}

namespace detail
{
std::optional<Nl80211WiphyBand>
ParseNl80211WiphyBand(struct nlattr *wiphyBand) noexcept
{
    // Parse the attribute message.
    std::array<struct nlattr *, NL80211_BAND_ATTR_MAX + 1> wiphyBandAttributes{};
    int ret = nla_parse(std::data(wiphyBandAttributes), std::size(wiphyBandAttributes), static_cast<struct nlattr *>(nla_data(wiphyBand)), nla_len(wiphyBand), nullptr);
    if (ret < 0) {
        LOGE << std::format("Failed to parse wiphy band attributes with error {} ({})", ret, nl_geterror(ret));
        return std::nullopt;
    }

    bool supportsRoaming = wiphyBandAttributes[NL80211_ATTR_ROAM_SUPPORT] != nullptr;

    return Nl80211WiphyBand(supportsRoaming);
}
} // namespace detail

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
    int ret = nla_parse(std::data(wiphyAttributes), std::size(wiphyAttributes), genlmsg_attrdata(genl80211MessageHeader, 0), genlmsg_attrlen(genl80211MessageHeader, 0), nullptr);
    if (ret < 0) {
        LOG_ERROR << std::format("Failed to parse netlink message attributes with error {} ({})", ret, strerror(-ret));
        return std::nullopt;
    }

    // Tease out parameters to populate the Nl80211Wiphy instance.
    auto wiphyIndex = static_cast<uint32_t>(nla_get_u32(wiphyAttributes[NL80211_ATTR_WIPHY]));
    auto wiphyName = static_cast<const char *>(nla_data(wiphyAttributes[NL80211_ATTR_WIPHY_NAME]));
    auto wiphyBands = static_cast<struct nlattr *>(nla_data(wiphyAttributes[NL80211_ATTR_WIPHY_BANDS]));
    auto wiphyNumCipherSuites = static_cast<std::size_t>(nla_len(wiphyAttributes[NL80211_ATTR_CIPHER_SUITES])) / sizeof(uint32_t);
    auto *wiphyCipherSuites = static_cast<uint32_t *>(nla_data(wiphyAttributes[NL80211_ATTR_CIPHER_SUITES]));
    if (wiphyBands == nullptr) {
        LOGE << "Received null wiphy bands";
        return std::nullopt;
    }

    int remainingBands;
    struct nlattr *wiphyBand;
    std::unordered_map<nl80211_band, Nl80211WiphyBand> wiphyBandMap{};
    nla_for_each_nested(wiphyBand, wiphyBands, remainingBands)
    {
        auto nl80211BandType = static_cast<nl80211_band>(wiphyBand->nla_type);
        auto nl80211Band = detail::ParseNl80211WiphyBand(wiphyBand);
        if (nl80211Band.has_value()) {
            wiphyBandMap.emplace(nl80211BandType, std::move(nl80211Band.value()));
        }
    }

    std::vector<uint32_t> cipherSuites(wiphyCipherSuites, wiphyCipherSuites + wiphyNumCipherSuites);
    Nl80211Wiphy nl80211Wiphy{ wiphyIndex, wiphyName, std::move(cipherSuites), std::move(wiphyBandMap) };

    return nl80211Wiphy;
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
    } else {
        LOGD << std::format("Parsed nl80211 wiphy message: {}", nl80211WiphyResult->ToString());
    }

    return NL_OK;
}
} // namespace detail

/* static */
std::optional<Nl80211Wiphy>
Nl80211Wiphy::FromIndex(uint32_t wiphyIndex)
{
    // Allocate a new netlink socket.
    auto nl80211SocketOpt{ CreateNl80211Socket() };
    if (!nl80211SocketOpt.has_value()) {
        LOGE << "Failed to create nl80211 socket";
        return std::nullopt;
    }

    // Allocate a new nl80211 message for sending the dump request for all interfaces.
    auto nl80211Socket{ std::move(nl80211SocketOpt.value()) };
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

    // Add the wiphy index attribute so nl80211 knows what to lookup.
    nla_put_u32(nl80211MessageGetWiphy, NL80211_ATTR_WIPHY, wiphyIndex);
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
