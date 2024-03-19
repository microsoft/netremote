
#include <memory>

#include <google/protobuf/empty.pb.h>
#include <grpcpp/server_context.h>
#include <grpcpp/support/server_callback.h>
#include <microsoft/net/remote/NetRemoteDataStreamingService.hxx>
#include <microsoft/net/remote/protocol/NetRemoteDataStream.pb.h>

#include "NetRemoteApiTrace.hxx"
#include "NetRemoteDataStreamingReactors.hxx"

using namespace Microsoft::Net::Remote::DataStream;
using namespace Microsoft::Net::Remote::Service;
using namespace Microsoft::Net::Remote::Service::Tracing;

grpc::ServerReadReactor<DataStreamUploadData>*
NetRemoteDataStreamingService::DataStreamUpload([[maybe_unused]] grpc::CallbackServerContext* context, DataStreamUploadResult* result)
{
    const NetRemoteApiTrace traceMe{};

    return std::make_unique<Reactors::DataStreamReader>(result).release();
}

grpc::ServerWriteReactor<DataStreamDownloadData>*
NetRemoteDataStreamingService::DataStreamDownload([[maybe_unused]] grpc::CallbackServerContext* context, const DataStreamDownloadRequest* request)
{
    const NetRemoteApiTrace traceMe{};

    return std::make_unique<Reactors::DataStreamWriter>(request).release();
}

grpc::ServerBidiReactor<DataStreamUploadData, DataStreamDownloadData>*
NetRemoteDataStreamingService::DataStreamBidirectional([[maybe_unused]] grpc::CallbackServerContext* context)
{
    const NetRemoteApiTrace traceMe{};

    return std::make_unique<Reactors::DataStreamReaderWriter>().release();
}

grpc::ServerUnaryReactor*
NetRemoteDataStreamingService::DataStreamPing(grpc::CallbackServerContext* context, [[maybe_unused]] const google::protobuf::Empty* request, [[maybe_unused]] google::protobuf::Empty* response)
{
    const NetRemoteApiTrace traceMe{};

    auto* reactor = context->DefaultReactor();
    reactor->Finish(grpc::Status::OK);

    return reactor;
}
