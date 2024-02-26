
#include "NetRemoteDataStreamingReactors.hxx"

using namespace Microsoft::Net::Remote::Service::Reactors;
using namespace Microsoft::Net::Remote::Wifi;

DataStreamReader::DataStreamReader(WifiDataStreamUploadResult* result) :
    m_result(result)
{
    StartRead(&m_data);
}

void
DataStreamReader::OnReadDone(bool isOk)
{
    if (isOk) {
        m_numberOfDataBlocksReceived++;
        m_readStatus.set_code(WifiDataStreamOperationStatusCode::WifiDataStreamOperationStatusCodeSucceeded);
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
    m_readStatus.set_code(WifiDataStreamOperationStatusCode::WifiDataStreamOperationStatusCodeCancelled);
    m_readStatus.set_message("RPC cancelled");
    *m_result->mutable_status() = std::move(m_readStatus);
    Finish(grpc::Status::CANCELLED);
}

void
DataStreamReader::OnDone()
{
    delete this;
}
