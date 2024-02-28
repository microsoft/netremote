
#include "NetRemoteDataStreamingReactors.hxx"

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
}

DataStreamWriter::DataStreamWriter(const DataStreamDownloadRequest* request)
{
    m_dataStreamProperties = request->properties();
    if (m_dataStreamProperties.type() == DataStreamType::Fixed) {
        // TODO: Ensure that m_dataStreamProperties.Value_case() == WifiDataStreamProperties.kFixedTypeProperties
        m_numberOfDataBlocksToStream = m_dataStreamProperties.fixedtypeproperties().numberofdatablockstostream();
    }
    m_writeStatus.set_code(DataStreamOperationStatusCode::DataStreamOperationStatusCodeUnknown);
    m_writeStatus.set_message("No data sent yet");
    NextWrite();
}

void
DataStreamWriter::OnWriteDone(bool ok)
{
    // Check for a failed status code from HandleWriteFailure since that invoked a final write, thus causing this callback to be invoked.
    if (m_writeStatus.code() == DataStreamOperationStatusCode::DataStreamOperationStatusCodeFailed) {
        Finish(::grpc::Status::OK);
        return;
    }

    if (ok) {
        if (m_dataStreamProperties.type() == DataStreamType::Fixed) {
            m_numberOfDataBlocksToStream--;
        }
        m_writeStatus.set_code(DataStreamOperationStatusCode::DataStreamOperationStatusCodeSucceeded);
        m_writeStatus.set_message("Data write successful");
        NextWrite();
    } else {
        HandleWriteFailure();
    }
}

void
DataStreamWriter::OnCancel()
{
}

void
DataStreamWriter::OnDone()
{
}

void
DataStreamWriter::NextWrite()
{
    if (m_dataStreamProperties.type() == DataStreamType::Continuous || m_numberOfDataBlocksToStream > 0) {
        const auto data = std::format("Data #{}", ++m_numberOfDataBlocksWritten);
        m_data.set_data(data);
        m_data.set_sequencenumber(m_numberOfDataBlocksWritten);
        *m_data.mutable_status() = m_writeStatus;
        StartWrite(&m_data);
    } else {
        // No more data to write
        Finish(::grpc::Status::OK);
    }
}

void
DataStreamWriter::HandleWriteFailure()
{
    m_writeStatus.set_code(DataStreamOperationStatusCode::DataStreamOperationStatusCodeFailed);
    m_writeStatus.set_message("Data write failed");
    StartWrite(&m_data);
}
