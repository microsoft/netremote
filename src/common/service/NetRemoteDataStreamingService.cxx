
#include "NetRemoteDataStreamingReactors.hxx"
#include <microsoft/net/remote/NetRemoteDataStreamingService.hxx>

using namespace Microsoft::Net::Remote::DataStream;
using namespace Microsoft::Net::Remote::Service;

grpc::ServerReadReactor<DataStreamUploadData>*
NetRemoteDataStreamingService::DataStreamUpload([[maybe_unused]] grpc::CallbackServerContext* context, DataStreamUploadResult* result)
{
    return new Reactors::DataStreamReader(result);
}
