
#ifndef ACCESS_POINT_OPERATION_STATUS_HXX
#define ACCESS_POINT_OPERATION_STATUS_HXX

#include <source_location>
#include <string>
#include <string_view>

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
    std::string AccessPointId;
    std::string OperationName;
    AccessPointOperationStatusCode Code{ AccessPointOperationStatusCode::Unknown };
    std::string Details;
    std::string Message; // TODO: remove this

    AccessPointOperationStatus() = default; // TODO: remove this
    virtual ~AccessPointOperationStatus() = default;

    /**
     * @brief Create an AccessPointOperationStatus with the given access point id and operation name.
     */
    constexpr AccessPointOperationStatus(std::string_view accessPointId, const char *operationName = std::source_location::current().function_name()) noexcept :
        AccessPointOperationStatus(accessPointId, AccessPointOperationStatusCode::Unknown, operationName)
    {}

    /**
     * @brief Create an AccessPointOperationStatus with the given access point id, operation name, and status code.
     */
    constexpr AccessPointOperationStatus(std::string_view accessPointId, AccessPointOperationStatusCode code, const char *operationName = std::source_location::current().function_name()) noexcept :
        AccessPointOperationStatus(accessPointId, code, "", operationName)
    {}

    /**
     * @brief Create an AccessPointOperationStatus with the given access point id, operation name, status code, and details.
     */
    constexpr AccessPointOperationStatus(std::string_view accessPointId, AccessPointOperationStatusCode code, std::string_view details, const char *operationName = std::source_location::current().function_name()) noexcept :
        AccessPointId{ accessPointId },
        OperationName{ operationName },
        Code{ code },
        Details{ details }
    {}

    AccessPointOperationStatus(const AccessPointOperationStatus &) = default;

    AccessPointOperationStatus(AccessPointOperationStatus &&) = default;

    AccessPointOperationStatus &
    operator=(const AccessPointOperationStatus &) = default;

    AccessPointOperationStatus &
    operator=(AccessPointOperationStatus &&) = default;

    /**
     * @brief Return a string representation of the status.
     *
     * @return std::string
     */
    std::string
    ToString() const;

    /**
     * @brief Create an AccessPointOperationStatus describing an operation that succeeded.
     *
     * @return AccessPointOperationStatus
     */
    static AccessPointOperationStatus
    MakeSucceeded(std::string_view accessPointId) noexcept;

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
