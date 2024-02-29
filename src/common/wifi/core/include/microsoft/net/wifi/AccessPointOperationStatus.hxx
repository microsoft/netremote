
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
    std::source_location SourceLocation;
    std::string AccessPointId;
    std::string OperationName;
    std::string Details;
    AccessPointOperationStatusCode Code{ AccessPointOperationStatusCode::Unknown };

    virtual ~AccessPointOperationStatus() = default;

    /**
     * @brief Create an AccessPointOperationStatus with the given access point id, operation name, status code, and details.
     */
    constexpr AccessPointOperationStatus(std::string_view accessPointId, std::string_view operationName = {}, AccessPointOperationStatusCode code = AccessPointOperationStatusCode::Unknown, std::string_view details = {}, std::source_location sourceLocation = std::source_location::current()) noexcept :
        SourceLocation{ sourceLocation },
        AccessPointId{ std::move(accessPointId) },
        OperationName{ operationName },
        Details{ details },
        Code{ code }
    {
        if (std::empty(OperationName)) {
            OperationName = SourceLocation.function_name();
            auto pos = OperationName.find_first_of('(');
            if (pos != std::string::npos) {
                OperationName.erase(pos);
            }
            pos = OperationName.find_last_of(':');
            if (pos != std::string::npos) {
                OperationName.erase(0, pos);
            }
        }
    }

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
     * @param accessPointId The ID of the access point.
     * @param operationName The name of the operation.
     * @param details Additional details about the operation.
     * @param sourceLocation The source location of the operation.
     * @return AccessPointOperationStatus
     */
    static AccessPointOperationStatus
    MakeSucceeded(std::string_view accessPointId, std::string_view operationName = {}, std::string_view details = {}, std::source_location sourceLocation = std::source_location::current()) noexcept;

    /**
     * @brief Create an AccessPointOperationStatus description an operation that referred to an invalid access point.
     *
     * @param operationName The name of the operation.
     * @param details Additional details about the operation.
     * @param sourceLocation The source location of the operation.
     * @return AccessPointOperationStatus
     */
    static AccessPointOperationStatus
    InvalidAccessPoint(std::string_view operationName = {}, std::string_view details = {}, std::source_location sourceLocation = std::source_location::current()) noexcept;

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
