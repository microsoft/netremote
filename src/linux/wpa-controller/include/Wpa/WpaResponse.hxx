
#ifndef WPA_RESPONSE_HXX
#define WPA_RESPONSE_HXX

#include <string>
#include <string_view>

namespace Wpa
{
/**
 * @brief Object to hold generic, unparsed data from a wpa_supplicant or hostapd
 * command response.
 */
struct WpaResponse
{
    /**
     * @brief Destroy the WpaResponse object.
     */
    virtual ~WpaResponse() = default;

    /**
     * @brief Construct an empty WpaResponse object.
     *
     */
    WpaResponse() = default;

    /**
     * @brief Construct a WpaResponse object with the specified payload.
     *
     * @param payload The payload of a WPA control command response.
     */
    explicit WpaResponse(std::string_view payload);

    /**
     * Prevent copying and moving of this object. 
     */
    WpaResponse(const WpaResponse&) = delete;
    WpaResponse& operator=(const WpaResponse&) = delete;
    WpaResponse(WpaResponse&&) = delete;
    WpaResponse& operator=(WpaResponse&&) = delete;

    /**
     * @brief Implicit bool operator allowing WpaResponse to be used directly in
     * condition statements (eg. if (response) // ...).
     *
     * @return true
     * @return false
     */
    operator bool() const; // NOLINT(hicpp-explicit-conversions)

    /**
     * @brief Indicates whether the response describes a successful result.
     *
     * @return true
     * @return false
     */
    bool
    IsOk() const;

    /**
     * @brief Indicates whether the response describes a failed result.
     *
     * @return true
     * @return false
     */
    bool
    Failed() const;

    /**
     * @brief Provides the response payload.
     * 
     * @return std::string_view 
     */
    std::string_view
    Payload() const;

private:
    std::string m_payload;
};
} // namespace Wpa

#endif // WPA_RESPONSE_HXX
