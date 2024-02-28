
#include <format>

#include "NetRemoteDataStreamingReactors.hxx"
#include <magic_enum.hpp>

using namespace Microsoft::Net::Remote::DataStream;
using namespace Microsoft::Net::Remote::Service::Reactors;

DataStreamReader::DataStreamReader(DataStreamUploadResult* result) :
    m_result(result)
{
    StartRead(&m_data);
}

void
DataStreamReader::OnReadDone(bool isOk)
{
    if (isOk) {
        m_numberOfDataBlocksReceived++;
        m_readStatus.set_code(DataStreamOperationStatusCode::DataStreamOperationStatusCodeSucceeded);
        m_readStatus.set_message("Data read successful");
        StartRead(&m_data);
    } else {
        // A false "isOk" value could either mean a failed RPC or that no more data is available.
        // Unfortunately, there is no clear way to tell which situation occurred.
        m_result->set_numberofdatablocksreceived(m_numberOfDataBlocksReceived);
        *m_result->mutable_status() = std::move(m_readStatus);
        Finish(grpc::Status::OK);
    }
}

void
DataStreamReader::OnCancel()
{
    m_result->set_numberofdatablocksreceived(m_numberOfDataBlocksReceived);
    m_readStatus.set_code(DataStreamOperationStatusCode::DataStreamOperationStatusCodeCancelled);
    m_readStatus.set_message("RPC cancelled");
    *m_result->mutable_status() = std::move(m_readStatus);
    Finish(grpc::Status::CANCELLED);
}

void
DataStreamReader::OnDone()
{
    delete this;
}

DataStreamWriter::DataStreamWriter(const DataStreamDownloadRequest* request)
{
    m_dataStreamProperties = request->properties();

    switch (m_dataStreamProperties.type()) {
    case DataStreamType::DataStreamTypeFixed: {
        if (m_dataStreamProperties.Properties_case() == DataStreamProperties::kFixedTypeProperties) {
            m_numberOfDataBlocksToStream = m_dataStreamProperties.fixedtypeproperties().numberofdatablockstostream();
        } else {
            HandleFailure("Invalid properties for this streaming type. Expected FixedTypeProperties for DataStreamTypeFixed");
            return;
        }
    }
    case DataStreamType::DataStreamTypeContinuous: {
        if (m_dataStreamProperties.Properties_case() == DataStreamProperties::kContinuousTypeProperties) {
            m_numberOfDataBlocksToStream = 0;
        } else {
            HandleFailure("Invalid properties for this streaming type. Expected ContinuousTypeProperties for DataStreamTypeContinuous");
            return;
        }
    }
    default: {
        HandleFailure(std::format("Invalid streaming type: {}", magic_enum::enum_name(m_dataStreamProperties.type())));
        return;
    }
    };

    m_writeStatus.set_code(DataStreamOperationStatusCode::DataStreamOperationStatusCodeUnknown);
    m_writeStatus.set_message("No data sent yet");
    NextWrite();
}

void
DataStreamWriter::OnWriteDone(bool isOk)
{
    // Check for a failed status code from HandleWriteFailure since that invoked a final write, thus causing this callback to be invoked.
    if (m_writeStatus.code() == DataStreamOperationStatusCode::DataStreamOperationStatusCodeFailed) {
        Finish(::grpc::Status::OK);
        return;
    }

    // Continue writing if previous write was successful, otherwise handle the failure.
    if (isOk) {
        if (m_dataStreamProperties.type() == DataStreamType::DataStreamTypeFixed) {
            m_numberOfDataBlocksToStream--;
        }
        m_writeStatus.set_code(DataStreamOperationStatusCode::DataStreamOperationStatusCodeSucceeded);
        m_writeStatus.set_message("Data write successful");
        NextWrite();
    } else {
        HandleFailure("Data write failed");
    }
}

void
DataStreamWriter::OnCancel()
{
    // The RPC is cancelled, so call Finish to complete it.
    Finish(grpc::Status::CANCELLED);
}

void
DataStreamWriter::OnDone()
{
    delete this;
}

void
DataStreamWriter::NextWrite()
{
    if (m_dataStreamProperties.type() == DataStreamType::DataStreamTypeContinuous ||
        (m_dataStreamProperties.type() == DataStreamType::DataStreamTypeFixed && m_numberOfDataBlocksToStream > 0)) {
        // Create data to write to the client.
        const auto data = std::format("Data #{}", ++m_numberOfDataBlocksWritten);

        // Write data to the client.
        m_data.set_data(data);
        m_data.set_sequencenumber(m_numberOfDataBlocksWritten);
        *m_data.mutable_status() = m_writeStatus;
        StartWrite(&m_data);
    } else {
        // No more data to write.
        Finish(::grpc::Status::OK);
    }
}

void
DataStreamWriter::HandleFailure(const std::string errorMessage)
{
    m_writeStatus.set_code(DataStreamOperationStatusCode::DataStreamOperationStatusCodeFailed);
    m_writeStatus.set_message(errorMessage);
    *m_data.mutable_status() = m_writeStatus;

    // Write a final message to the client. The OnWriteDone() callback will check for the
    // DataStreamOperationStatusCodeFailed status code set here to know to complete the RPC.
    StartWrite(&m_data);
}
