
#include <source_location>

#include <logging/FunctionTracer.hxx>
#include <plog/Severity.h>

#include "NetRemoteApiTrace.hxx"

using namespace Microsoft::Net::Remote::Service::Tracing;

NetRemoteApiTrace::NetRemoteApiTrace(bool deferEnter, plog::Severity logSeverityEnter, plog::Severity logSeverityExit, std::source_location location) :
    logging::FunctionTracer(logSeverityEnter, logSeverityExit, LogTracePrefix, {}, deferEnter, location)
{}
