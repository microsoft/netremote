
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

    m_dataStreamReaders.push_back(std::make_unique<Reactors::DataStreamReader>(result));

    return m_dataStreamReaders.back().get();
}

grpc::ServerWriteReactor<DataStreamDownloadData>*
NetRemoteDataStreamingService::DataStreamDownload([[maybe_unused]] grpc::CallbackServerContext* context, const DataStreamDownloadRequest* request)
{
    const NetRemoteApiTrace traceMe{};

    m_dataStreamWriter = std::make_unique<Reactors::DataStreamWriter>(request);

    return m_dataStreamWriter.get();
}
