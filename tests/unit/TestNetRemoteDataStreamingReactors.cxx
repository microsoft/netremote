
#include <cstddef>
#include <cstdint>
#include <format>
#include <mutex>
#include <span>
#include <utility>

#include <grpcpp/impl/codegen/status.h>
#include <magic_enum.hpp>
#include <microsoft/net/remote/protocol/NetRemoteDataStream.pb.h>
#include <microsoft/net/remote/protocol/NetRemoteDataStreamingService.grpc.pb.h>
#include <plog/Log.h>

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
    const std::unique_lock lock(m_writeStatusGate);

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
        m_numberOfDataBlocksToWrite--;
        StartWrite(&m_data);
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

        // Keep track of the sequence numbers of data blocks that were not received.
        if (m_data.sequencenumber() != m_numberOfDataBlocksReceived) {
            auto numberOfLostDataBlocks = m_data.sequencenumber() - m_numberOfDataBlocksReceived;
            for (uint32_t i = numberOfLostDataBlocks; i > 0; i--) {
                m_lostDataBlockSequenceNumbers.push_back(m_data.sequencenumber() - i);
            }
        }
        StartRead(&m_data);
    }
    // If read fails, then there is likely no more data to be read, so do nothing.
}

void
DataStreamReader::OnDone(const grpc::Status& status)
{
    const std::unique_lock lock(m_readStatusGate);

    m_status = status;
    m_done = true;
    m_readsDone.notify_one();
}

grpc::Status
DataStreamReader::Await(uint32_t* numberOfDataBlocksReceived, DataStreamOperationStatus* operationStatus, std::span<uint32_t> lostDataBlockSequenceNumbers)
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

    *numberOfDataBlocksReceived = m_numberOfDataBlocksReceived;
    *operationStatus = m_data.status();
    lostDataBlockSequenceNumbers = std::span<uint32_t>(std::data(m_lostDataBlockSequenceNumbers), std::size(m_lostDataBlockSequenceNumbers));

    return m_status;
}

void
DataStreamReader::Cancel()
{
    LOGD << "Attempting to cancel RPC";
    m_clientContext.TryCancel();
}

DataStreamReaderWriter::DataStreamReaderWriter(NetRemoteDataStreaming::Stub* client, DataStreamProperties dataStreamProperties) :
    m_dataStreamProperties(dataStreamProperties)
{
    switch (m_dataStreamProperties.type()) {
    case DataStreamType::DataStreamTypeFixed: {
        if (m_dataStreamProperties.Properties_case() == DataStreamProperties::kFixed) {
            m_numberOfDataBlocksToWrite = m_dataStreamProperties.fixed().numberofdatablockstostream();
        } else {
            LOGE << "Invalid properties for this streaming type. Expected Fixed for DataStreamTypeFixed";
            return;
        }

        break;
    }
    case DataStreamType::DataStreamTypeContinuous: {
        if (m_dataStreamProperties.Properties_case() == DataStreamProperties::kContinuous) {
            m_numberOfDataBlocksToWrite = 0;
        } else {
            LOGE << "Invalid properties for this streaming type. Expected Continuous for DataStreamTypeContinuous";
            return;
        }

        break;
    }
    default: {
        LOGE << std::format("Invalid streaming type: {}", magic_enum::enum_name(m_dataStreamProperties.type()));
        return;
    }
    };

    client->async()->DataStreamBidirectional(&m_clientContext, this);
    StartCall();
    StartRead(&m_readData);
    NextWrite();
}

void
DataStreamReaderWriter::OnReadDone(bool isOk)
{
    if (isOk) {
        m_numberOfDataBlocksReceived++;

        // Keep track of the sequence numbers of data blocks that were not received.
        if (m_readData.sequencenumber() != m_numberOfDataBlocksReceived) {
            auto numberOfLostDataBlocks = m_readData.sequencenumber() - m_numberOfDataBlocksReceived;
            for (uint32_t i = numberOfLostDataBlocks; i > 0; i--) {
                m_lostDataBlockSequenceNumbers.push_back(m_readData.sequencenumber() - i);
            }
        }
        StartRead(&m_readData);
    }
}

void
DataStreamReaderWriter::OnWriteDone(bool isOk)
{
    if (isOk) {
        if (m_dataStreamProperties.type() == DataStreamType::DataStreamTypeFixed) {
            m_numberOfDataBlocksToWrite--;
        }
        NextWrite();
    } else {
        StartWritesDone();
    }
}

void
DataStreamReaderWriter::OnDone(const grpc::Status& status)
{
    const std::unique_lock lock(m_operationStatusGate);

    m_operationStatus = status;
    m_done = true;
    m_operationsDone.notify_one();
}

grpc::Status
DataStreamReaderWriter::Await(uint32_t* numberOfDataBlocksReceived, DataStreamOperationStatus* operationStatus, std::span<uint32_t> lostDataBlockSequenceNumbers)
{
    std::unique_lock lock(m_operationStatusGate);

    const auto isDone = m_operationsDone.wait_for(lock, DefaultTimeoutValue, [this] {
        return m_done;
    });

    // Handle timeout from waiting for reads to be completed.
    if (!isDone) {
        DataStreamOperationStatus status{};
        status.set_code(DataStreamOperationStatusCode::DataStreamOperationStatusCodeTimedOut);
        status.set_message("Timeout occurred while waiting for all operations to be completed");
        *m_readData.mutable_status() = std::move(status);
    }

    *numberOfDataBlocksReceived = m_numberOfDataBlocksReceived;
    *operationStatus = m_readData.status();
    lostDataBlockSequenceNumbers = std::span<uint32_t>(std::data(m_lostDataBlockSequenceNumbers), std::size(m_lostDataBlockSequenceNumbers));

    return m_operationStatus;
}

void
DataStreamReaderWriter::Cancel()
{
    LOGD << "Attempting to cancel RPC";
    m_clientContext.TryCancel();
}

void
DataStreamReaderWriter::NextWrite()
{
    if (m_dataStreamProperties.type() == DataStreamType::DataStreamTypeContinuous ||
        (m_dataStreamProperties.type() == DataStreamType::DataStreamTypeFixed && m_numberOfDataBlocksToWrite > 0)) {
        m_writeData.set_data(std::format("Data #{}", ++m_numberOfDataBlocksWritten));
        StartWrite(&m_writeData);
    } else {
        StartWritesDone();
    }
}
