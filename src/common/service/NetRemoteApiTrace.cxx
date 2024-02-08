
#include <source_location>

#include "NetRemoteApiTrace.hxx"
#include <logging/FunctionTracer.hxx>

using namespace Microsoft::Net::Remote::Service::Tracing;

NetRemoteApiTrace::NetRemoteApiTrace(bool deferEnter, std::source_location location) :
    logging::FunctionTracer(LogTracePrefix, {}, deferEnter, location)
{}
