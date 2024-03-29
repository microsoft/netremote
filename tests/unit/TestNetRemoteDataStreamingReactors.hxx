
#ifndef TEST_NET_REMOTE_DATA_STREAMING_REACTORS_HXX
#define TEST_NET_REMOTE_DATA_STREAMING_REACTORS_HXX

#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <mutex>
#include <span>
#include <vector>

#include <microsoft/net/remote/protocol/NetRemoteDataStream.pb.h>
#include <microsoft/net/remote/protocol/NetRemoteDataStreamingService.grpc.pb.h>

using namespace std::chrono_literals;

namespace Microsoft::Net::Remote::Test
{
/**
 * @brief Implementation of the gRPC ClientWriteReactor for client-side data stream writing.
 */
class DataStreamWriter :
    public grpc::ClientWriteReactor<Microsoft::Net::Remote::DataStream::DataStreamUploadData>
{
public:
    /**
     * @brief Construct a new DataStreamWriter object with the client stub and specified number of data blocks to write.
     *
     * @param client The data streaming client stub.
     * @param numberOfDataBlocksToWrite The number of data blocks to write.
     */
    explicit DataStreamWriter(Microsoft::Net::Remote::Service::NetRemoteDataStreaming::Stub* client, uint32_t numberOfDataBlocksToWrite);

    /**
     * @brief Callback that is executed when a write operation is completed.
     *
     * @param isOk Indicates whether a write was successfully sent.
     */
    void
    OnWriteDone(bool isOk) override;

    /**
     * @brief Callback that is executed when all RPC operations are completed for a given RPC.
     *
     * @param status The status of the RPC sent by the server or provided by the library to indicate a failure.
     */
    void
    OnDone(const grpc::Status& status) override;

    /**
     * @brief Wait for all operations to complete and transfer the result to the specific result output parameter.
     *
     * @param result The result of the data stream write operation.
     * @return grpc::Status
     */
    grpc::Status
    Await(Microsoft::Net::Remote::DataStream::DataStreamUploadResult* result);

private:
    /**
     * @brief Facilitate the next write operation.
     */
    void
    NextWrite();

private:
    static inline constexpr auto DefaultTimeoutValue{ 10s };

    grpc::ClientContext m_clientContext{};
    Microsoft::Net::Remote::DataStream::DataStreamUploadData m_data{};
    Microsoft::Net::Remote::DataStream::DataStreamUploadResult m_result{};
    uint32_t m_numberOfDataBlocksToWrite{};
    uint32_t m_numberOfDataBlocksWritten{};
    grpc::Status m_status{};
    std::mutex m_writeStatusGate{};
    std::condition_variable m_writesDone{};
    bool m_done{ false };
    std::chrono::duration<uint32_t> m_writesDoneTimeoutValue{ DefaultTimeoutValue };
};

/**
 * @brief Implementation of the gRPC ClientReadReactor for client-side data stream reading.
 */
class DataStreamReader :
    public grpc::ClientReadReactor<Microsoft::Net::Remote::DataStream::DataStreamDownloadData>
{
public:
    /**
     * @brief Construct a new DataStreamReader object with the client stub and specified download request.
     *
     * @param client The data streaming client stub.
     * @param request The download request to be sent to the server.
     */
    explicit DataStreamReader(Microsoft::Net::Remote::Service::NetRemoteDataStreaming::Stub* client, Microsoft::Net::Remote::DataStream::DataStreamDownloadRequest* request);

    /**
     * @brief Callback that is executed when a read operation is completed.
     *
     * @param isOk Indicates whether a message was read as expected.
     */
    void
    OnReadDone(bool isOk) override;

    /**
     * @brief Callback that is executed when all RPC operations are completed for a given RPC.
     *
     * @param status The status of the RPC sent by the server or provided by the library to indicate a failure.
     */
    void
    OnDone(const grpc::Status& status) override;

    /**
     * @brief Wait for all operations to complete and transfer the resulting information to the output parameters.
     *
     * @param numberOfDataBlocksReceived The number of data blocks received by the client.
     * @param operationStatus The status of the data stream read operation.
     * @param lostDataBlockSequenceNumbers The list of sequence numbers associated with data blocks sent by the server that were not received by the client.
     * @return grpc::Status
     */
    grpc::Status
    Await(uint32_t* numberOfDataBlocksReceived, Microsoft::Net::Remote::DataStream::DataStreamOperationStatus* operationStatus, std::span<uint32_t>& lostDataBlockSequenceNumbers);

    /**
     * @brief Cancel the ongoing RPC.
     */
    void
    Cancel();

private:
    static inline constexpr auto DefaultTimeoutValue{ 10s };

    grpc::ClientContext m_clientContext{};
    Microsoft::Net::Remote::DataStream::DataStreamDownloadData m_data{};
    uint32_t m_numberOfDataBlocksReceived{};
    grpc::Status m_status{};
    std::mutex m_readStatusGate{};
    std::condition_variable m_readsDone{};
    bool m_done{ false };
    std::chrono::duration<uint32_t> m_readsDoneTimeoutValue{ DefaultTimeoutValue };
    std::vector<uint32_t> m_lostDataBlockSequenceNumbers{};
};

/**
 * @brief Implementation of the gRPC ClientBidiReactor for client-side data stream reading and writing.
 */
class DataStreamReaderWriter :
    public grpc::ClientBidiReactor<Microsoft::Net::Remote::DataStream::DataStreamUploadData, Microsoft::Net::Remote::DataStream::DataStreamDownloadData>
{
public:
    /**
     * @brief Construct a new DataStreamReaderWriter object with the client stub and specified number of data blocks to write.
     *
     * @param client The data streaming client stub.
     * @param dataStreamProperties The properties associated with the type of data streaming.
     */
    explicit DataStreamReaderWriter(Microsoft::Net::Remote::Service::NetRemoteDataStreaming::Stub* client, Microsoft::Net::Remote::DataStream::DataStreamProperties dataStreamProperties);

    /**
     * @brief Callback that is executed when a read operation is completed.
     *
     * @param isOk Indicates whether a message was read as expected.
     */
    void
    OnReadDone(bool isOk) override;

    /**
     * @brief Callback that is executed when a write operation is completed.
     *
     * @param isOk Indicates whether a write was successfully sent.
     */
    void
    OnWriteDone(bool isOk) override;

    /**
     * @brief Callback that is executed when all RPC operations are completed for a given RPC.
     *
     * @param status The status of the RPC sent by the server or provided by the library to indicate a failure.
     */
    void
    OnDone(const grpc::Status& status) override;

    /**
     * @brief Wait for all operations to complete and transfer the status to the status output parameter.
     *
     * @param numberOfDataBlocksReceived The number of data blocks received by the client.
     * @param operationStatus The status of the read/write data operations.
     * @param lostDataBlockSequenceNumbers The list of sequence numbers associated with data blocks sent by the server that were not received by the client.
     * @return grpc::Status
     */
    grpc::Status
    Await(uint32_t* numberOfDataBlocksReceived, Microsoft::Net::Remote::DataStream::DataStreamOperationStatus* operationStatus, std::span<uint32_t>& lostDataBlockSequenceNumbers);

    /**
     * @brief Stops writing data to the client. Should only be called with DataStreamTypeContinuous.
     */
    void
    StopWrites();

private:
    /**
     * @brief Facilitate the next write operation.
     */
    void
    NextWrite();

private:
    static inline constexpr auto DefaultTimeoutValue{ 10s };

    grpc::ClientContext m_clientContext{};
    Microsoft::Net::Remote::DataStream::DataStreamDownloadData m_readData{};
    Microsoft::Net::Remote::DataStream::DataStreamUploadData m_writeData{};
    Microsoft::Net::Remote::DataStream::DataStreamProperties m_dataStreamProperties{};
    uint32_t m_numberOfDataBlocksToWrite{};
    uint32_t m_numberOfDataBlocksWritten{};
    uint32_t m_numberOfDataBlocksReceived{};
    grpc::Status m_operationStatus{};
    std::mutex m_operationStatusGate{};
    std::condition_variable m_operationsDone{};
    bool m_done{ false };
    std::vector<uint32_t> m_lostDataBlockSequenceNumbers{};
    std::atomic<bool> m_writesStopped{};
};

} // namespace Microsoft::Net::Remote::Test

#endif // TEST_NET_REMOTE_DATA_STREAMING_REACTORS_HXX
