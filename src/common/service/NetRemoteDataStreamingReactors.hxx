
#ifndef NET_REMOTE_DATA_STREAMING_REACTORS_HXX
#define NET_REMOTE_DATA_STREAMING_REACTORS_HXX

#include <atomic>
#include <cstdint>
#include <string>

#include <microsoft/net/remote/protocol/NetRemoteDataStream.pb.h>
#include <microsoft/net/remote/protocol/NetRemoteDataStreamingService.grpc.pb.h>

namespace Microsoft::Net::Remote::Service::Reactors
{
/**
 * @brief Implementation of the gRPC ServerReadReactor for server-side data stream reading.
 */
class DataStreamReader :
    public grpc::ServerReadReactor<Microsoft::Net::Remote::DataStream::DataStreamUploadData>
{
public:
    /**
     * @brief Construct a new DataStreamReader object with the specified data stream upload result output parameter.
     *
     * @param result The result of the data stream read operation.
     */
    explicit DataStreamReader(Microsoft::Net::Remote::DataStream::DataStreamUploadResult* result);

    /**
     * @brief Callback that is executed when a read operation is completed.
     *
     * @param isOk Indicates whether a message was read as expected.
     */
    void
    OnReadDone(bool isOk) override;

    /**
     * @brief Callback that is executed when an RPC is cancelled before successfully sending a status to the client.
     */
    void
    OnCancel() override;

    /**
     * @brief Callback that is executed when all RPC operations are completed for a given RPC.
     */
    void
    OnDone() override;

private:
    Microsoft::Net::Remote::DataStream::DataStreamUploadData m_data{};
    Microsoft::Net::Remote::DataStream::DataStreamUploadResult* m_result{};
    uint32_t m_numberOfDataBlocksReceived{};
    Microsoft::Net::Remote::DataStream::DataStreamOperationStatus m_readStatus{};
};

/**
 * @brief Implementation of the gRPC ServerWriteReactor for server-side data stream writing.
 */
class DataStreamWriter :
    public grpc::ServerWriteReactor<Microsoft::Net::Remote::DataStream::DataStreamDownloadData>
{
public:
    /**
     * @brief Construct a new DataStreamWriter object with the specified download request.
     *
     * @param request The download request from the client.
     */
    explicit DataStreamWriter(const Microsoft::Net::Remote::DataStream::DataStreamDownloadRequest* request);

    /**
     * @brief Callback that is executed when a write operation is completed.
     *
     * @param isOk Indicates whether a write was successfully sent.
     */
    void
    OnWriteDone(bool isOk) override;

    /**
     * @brief Callback that is executed when an RPC is cancelled before successfully sending a status to the client.
     */
    void
    OnCancel() override;

    /**
     * @brief Callback that is executed when all RPC operations are completed for a given RPC.
     */
    void
    OnDone() override;

private:
    /**
     * @brief Facilitate the next write operation.
     */
    void
    NextWrite();

    /**
     * @brief Handle a failed operation.
     *
     * @param errorMessage The error message associated with the failed operation.
     */
    void
    HandleFailure(const std::string& errorMessage);

private:
    Microsoft::Net::Remote::DataStream::DataStreamDownloadData m_data{};
    Microsoft::Net::Remote::DataStream::DataStreamProperties m_dataStreamProperties{};
    uint32_t m_numberOfDataBlocksToStream{};
    uint32_t m_numberOfDataBlocksWritten{};
    Microsoft::Net::Remote::DataStream::DataStreamOperationStatus m_writeStatus{};
    std::atomic<bool> m_isCancelled{};
};
} // namespace Microsoft::Net::Remote::Service::Reactors

#endif // NET_REMOTE_DATA_STREAMING_REACTORS_HXX
