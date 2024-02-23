
#ifndef ACCESS_POINT_OPERATION_STATUS_HXX
#define ACCESS_POINT_OPERATION_STATUS_HXX

#include <string>

namespace Microsoft::Net::Wifi
{
/**
 * @brief Operational state of an access point.
 */
enum class AccessPointOperationalState : bool {
    Disabled = false,
    Enabled = true,
};

/**
 * @brief High-level status reported by various access point operations.
 */
enum class AccessPointOperationStatusCode {
    Unknown,
    Succeeded,
    InvalidParameter,
    AccessPointInvalid,
    AccessPointNotEnabled,
    OperationNotSupported,
    InternalError,
};

/**
 * @brief Coarse status of an access point operation.
 */
struct AccessPointOperationStatus
{
    AccessPointOperationStatusCode Code{ AccessPointOperationStatusCode::Unknown };
    std::string Message;

    AccessPointOperationStatus() = default;
    virtual ~AccessPointOperationStatus() = default;

    /**
     * @brief Create an AccessPointOperationStatus with the given status code.
     */
    constexpr explicit AccessPointOperationStatus(AccessPointOperationStatusCode code) noexcept :
        Code{ code }
    {}

    AccessPointOperationStatus(const AccessPointOperationStatus &) = default;
    AccessPointOperationStatus(AccessPointOperationStatus &&) = default;
    AccessPointOperationStatus &
    operator=(const AccessPointOperationStatus &) = default;
    AccessPointOperationStatus &
    operator=(AccessPointOperationStatus &&) = default;

    /**
     * @brief Create an AccessPointOperationStatus describing an operation that succeeded.
     *
     * @return AccessPointOperationStatus
     */
    static AccessPointOperationStatus
    MakeSucceeded() noexcept;

    /**
     * @brief Determine whether the operation succeeded.
     * 
     * @return true 
     * @return false 
     */
    bool
    Succeeded() const noexcept;

   /**
     * @brief Determine whether the operation failed.
    * 
    * @return true 
    * @return false 
    */
    bool
    Failed() const noexcept;

    /**
     * @brief Implicit bool operator allowing AccessPointOperationStatus to be used directly in condition statements
     * (eg. if (status) // ...).
     *
     * @return true
     * @return false
     */
    operator bool() const noexcept; // NOLINT(hicpp-explicit-conversions)
};
} // namespace Microsoft::Net::Wifi

#endif // ACCESS_POINT_OPERATION_STATUS_HXX
