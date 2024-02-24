
#ifndef NET_REMOTE_DATA_STREAMING_REACTORS_HXX
#define NET_REMOTE_DATA_STREAMING_REACTORS_HXX

#include <microsoft/net/remote/protocol/NetRemoteDataStreamingService.grpc.pb.h>

namespace Microsoft::Net::Remote::Service::Reactors
{
/**
 * @brief Implementation of the gRPC ServerReadReactor for server-side data stream reading.
 */
class DataStreamReader :
    public grpc::ServerReadReactor<Microsoft::Net::Remote::Wifi::WifiDataStreamUploadData>
{
public:
    /**
     * @brief Construct a new DataStreamReader object with the specified data stream upload result output parameter.
     *
     * @param result The result of the data stream read operation.
     */
    explicit DataStreamReader(Microsoft::Net::Remote::Wifi::WifiDataStreamUploadResult* result);

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
    Microsoft::Net::Remote::Wifi::WifiDataStreamUploadData m_data{};
    Microsoft::Net::Remote::Wifi::WifiDataStreamUploadResult* m_result{};
    uint32_t m_numberOfDataBlocksReceived{};
    Microsoft::Net::Remote::Wifi::WifiDataStreamOperationStatus m_readStatus{};
};
} // namespace Microsoft::Net::Remote::Service::Reactors

#endif // NET_REMOTE_DATA_STREAMING_REACTORS_HXX
