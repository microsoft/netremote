
#ifndef NET_REMOTE_API_TRACE_HXX
#define NET_REMOTE_API_TRACE_HXX

#include <source_location>

#include <logging/FunctionTracer.hxx>

namespace Microsoft::Net::Remote::Service::Tracing
{
/**
 * @brief Netremote API function tracer.
 */
struct NetRemoteApiTrace :
    public logging::FunctionTracer
{
    /**
     * @brief Construct a new NetRemoteApiTrace object.
     *
     * @param deferEnter Whether to defer the entry log message upon construction.
     * @param location The source code location of the function call.
     */
    NetRemoteApiTrace(bool deferEnter = false, std::source_location location = std::source_location::current());

private:
    static constexpr auto LogTracePrefix = "[API]";
};

} // namespace Microsoft::Net::Remote::Service::Tracing

#endif // NET_REMOTE_API_TRACE_HXX