
#include <chrono>

#include "TestNetRemoteDataStreamingReactors.hxx"

using namespace Microsoft::Net::Remote::DataStream;
using namespace Microsoft::Net::Remote::Service;
using namespace Microsoft::Net::Remote::Test;

using namespace std::chrono_literals;

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
    static constexpr auto timeoutValue = 10s;

    const auto isDone = m_writesDone.wait_for(lock, timeoutValue, [this] {
        return m_done;
    });

    if (!isDone) {
        DataStreamOperationStatus status{};
        status.set_code(DataStreamOperationStatusCode::DataStreamOperationStatusCodeTimedOut);
        status.set_message("Timeout occurred while waiting for all writes to be completed");
        *m_result.mutable_status() = std::move(status);
    }
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
