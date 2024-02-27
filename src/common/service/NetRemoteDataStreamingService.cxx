
#include "NetRemoteApiTrace.hxx"
#include "NetRemoteDataStreamingReactors.hxx"
#include <microsoft/net/remote/NetRemoteDataStreamingService.hxx>

using namespace Microsoft::Net::Remote::DataStream;
using namespace Microsoft::Net::Remote::Service;
using namespace Microsoft::Net::Remote::Service::Tracing;

grpc::ServerReadReactor<DataStreamUploadData>*
NetRemoteDataStreamingService::DataStreamUpload([[maybe_unused]] grpc::CallbackServerContext* context, DataStreamUploadResult* result)
{
    const NetRemoteApiTrace traceMe{};

    return new Reactors::DataStreamReader(result);
}
