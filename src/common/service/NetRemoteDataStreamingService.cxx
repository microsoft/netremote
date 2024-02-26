
#include "NetRemoteDataStreamingReactors.hxx"
#include <microsoft/net/remote/NetRemoteDataStreamingService.hxx>

using namespace Microsoft::Net::Remote::Service;
using namespace Microsoft::Net::Remote::Wifi;

grpc::ServerReadReactor<WifiDataStreamUploadData>*
NetRemoteDataStreamingService::WifiDataStreamUpload([[maybe_unused]] grpc::CallbackServerContext* context, WifiDataStreamUploadResult* result)
{
    return new Reactors::DataStreamReader(result);
}
