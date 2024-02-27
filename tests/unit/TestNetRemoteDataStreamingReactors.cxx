
#include "TestNetRemoteDataStreamingReactors.hxx"

using namespace Microsoft::Net::Remote::DataStream;
using namespace Microsoft::Net::Remote::Service;
using namespace Microsoft::Net::Remote::Test;

DataStreamWriter::DataStreamWriter(NetRemoteDataStreaming::Stub* client, uint32_t numberOfDataBlocksToWrite) :
    m_numberOfDataBlocksToWrite(numberOfDataBlocksToWrite)
{
    client->async()->DataStreamUpload(&m_clientContext, &m_result, this);
    StartCall();
    NextWrite();
}

void
DataStreamWriter::OnWriteDone(bool isOk)
{
    if (isOk) {
        NextWrite();
    } else {
        StartWritesDone();
    }
}

void
DataStreamWriter::OnDone(const grpc::Status& status)
{
    std::unique_lock lock(m_writeStatusGate);

    m_status = status;
    m_done = true;
    m_writesDone.notify_one();
}

grpc::Status
DataStreamWriter::Await(DataStreamUploadResult* result)
{
    std::unique_lock lock(m_writeStatusGate);
    static constexpr auto timeoutValue = std::chrono::seconds(10);

    m_writesDone.wait_for(lock, timeoutValue, [this] {
        return m_done;
    });
    *result = m_result;

    return m_status;
}

void
DataStreamWriter::NextWrite()
{
    if (m_numberOfDataBlocksToWrite > 0) {
        m_data.set_data(std::format("Data #{}", ++m_numberOfDataBlocksWritten));
        StartWrite(&m_data);
        m_numberOfDataBlocksToWrite--;
    } else {
        StartWritesDone();
    }
}
