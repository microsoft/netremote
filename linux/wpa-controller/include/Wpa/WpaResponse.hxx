
#ifndef WPA_RESPONSE_HXX
#define WPA_RESPONSE_HXX

#include <string_view>
#include <string>

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
    WpaResponse(std::string_view payload);

    /**
     * @brief Implicit bool operator allowing WpaResponse to be used directly in
     * condition statements (eg. if (response) // ...).
     * 
     * @return true 
     * @return false 
     */
    operator bool() const;

    /**
     * @brief Indicates whether the response describes a successful result.
     * 
     * @return true 
     * @return false 
     */
    bool IsOk() const;

    /**
     * @brief Indicates whether the response describes a failed result.
     * 
     * @return true 
     * @return false 
     */
    bool Failed() const;

private:
    /**
     * @brief Note: this must be declared prior to Payload since Payload is
     * initialized with its contents and the constructor initialization order
     * follows the order of declaration irrespective of the order of
     * initialization in the constructor.
     */
    const std::string m_payload;

public:
    std::string_view Payload;
};
} // namespace Wpa

#endif // WPA_RESPONSE_HXX
