
#ifndef ACCESS_POINT_OPERATION_STATUS_LOG_ON_EXIT_HXX
#define ACCESS_POINT_OPERATION_STATUS_LOG_ON_EXIT_HXX

#include <microsoft/net/wifi/AccessPointOperationStatus.hxx>
#include <plog/Severity.h>

namespace Microsoft::Net::Wifi
{
/**
 * @brief Helper to log an AccessPointOperationStatus on scope exit.
 */
struct AccessPointOperationStatusLogOnExit final
{
    AccessPointOperationStatus* OperationStatus;
    plog::Severity SeverityOnError{ plog::Severity::error };
    plog::Severity SeverityOnSuccess{ plog::Severity::debug };

    /**
     * @brief Construct a new AccessPointOperationStatusLogOnExit object with the given AccessPointOperationStatus and logging severities.
     *
     * @param operationStatus The AccessPointOperationStatus to log on exit.
     * @param severityOnError The severity to log on error.
     * @param severityOnSuccess The severity to log on success.
     */
    AccessPointOperationStatusLogOnExit(AccessPointOperationStatus* operationStatus, plog::Severity severityOnError = plog::Severity::error, plog::Severity severityOnSuccess = plog::Severity::info) noexcept;

    /**
     * @brief Destroy the object and log the stored AccessPointOperationStatus.
     */
    ~AccessPointOperationStatusLogOnExit();

    /**
     * @brief Reset the stored status object, preventing it from being logged on exit.
     */
    void
    Reset() noexcept;

    /**
     * @brief Prevent copying and moving of AccessPointOperationStatusLogOnExit objects.
     */
    AccessPointOperationStatusLogOnExit(const AccessPointOperationStatusLogOnExit&) = delete;

    AccessPointOperationStatusLogOnExit&
    operator=(const AccessPointOperationStatusLogOnExit&) = delete;

    AccessPointOperationStatusLogOnExit&
    operator=(AccessPointOperationStatusLogOnExit&& other) = delete;
};
} // namespace Microsoft::Net::Wifi

#endif // ACCESS_POINT_OPERATION_STATUS_LOG_ON_EXIT_HXX
