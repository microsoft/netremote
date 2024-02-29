
#include <chrono>
#include <format>

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

    const auto isDone = m_writesDone.wait_for(lock, m_writesDoneTimeoutValue, [this] {
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

DataStreamReader::DataStreamReader(NetRemoteDataStreaming::Stub* client, DataStreamDownloadRequest* request)
{
    client->async()->DataStreamDownload(&m_clientContext, request, this);
    StartCall();
    StartRead(&m_data);
}

void
DataStreamReader::OnReadDone(bool isOk)
{
    if (isOk) {
        m_numberOfDataBlocksReceived++;
        StartRead(&m_data);
    }
    // If read fails, then there is likely no more data to be read, so do nothing.
}

void
DataStreamReader::OnDone(const grpc::Status& status)
{
    std::unique_lock lock(m_readStatusGate);

    m_status = status;
    m_done = true;
    m_readsDone.notify_one();
}

grpc::Status
DataStreamReader::Await(uint32_t* numberOfDataBlocksReceived, DataStreamOperationStatus* operationStatus)
{
    std::unique_lock lock(m_readStatusGate);

    const auto isDone = m_readsDone.wait_for(lock, m_readsDoneTimeoutValue, [this] {
        return m_done;
    });

    // Handle timeout from waiting for reads to be completed.
    if (!isDone) {
        DataStreamOperationStatus status{};
        status.set_code(DataStreamOperationStatusCode::DataStreamOperationStatusCodeTimedOut);
        status.set_message("Timeout occurred while waiting for all reads to be completed");
        *m_data.mutable_status() = std::move(status);
    }

    // Handle mismatched sequence number and number of data blocks received.
    if (m_data.sequencenumber() != m_numberOfDataBlocksReceived) {
        DataStreamOperationStatus status{};
        status.set_code(DataStreamOperationStatusCode::DataStreamOperationStatusCodeFailed);
        status.set_message(std::format("Sequence number {} does not match the number of data blocks received {}", m_data.sequencenumber(), m_numberOfDataBlocksReceived));
        *m_data.mutable_status() = std::move(status);
    }

    *numberOfDataBlocksReceived = m_numberOfDataBlocksReceived;
    *operationStatus = m_data.status();

    return m_status;
}
