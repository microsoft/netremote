
#include <format>

#include "NetRemoteDataStreamingReactors.hxx"
#include <logging/FunctionTracer.hxx>
#include <magic_enum.hpp>
#include <plog/Log.h>
#include <plog/Severity.h>

namespace Microsoft::Net::Remote::Service::Reactors::Helpers
{
DataGenerator::DataGenerator()
{
    m_generator.seed(std::random_device{}());
}

std::string
DataGenerator::GenerateRandomData(const std::size_t length)
{
    std::string result;
    result.reserve(length);

    for (std::size_t i = 0; i < length; i++) {
        result.push_back(static_cast<char>(GetRandomByte()));
    }

    return result;
}

uint8_t
DataGenerator::GetRandomByte()
{
    std::uniform_int_distribution<uint32_t> distribution(0, std::numeric_limits<uint8_t>::max());
    return static_cast<uint8_t>(distribution(m_generator));
}
} // namespace Microsoft::Net::Remote::Service::Reactors::Helpers

using namespace Microsoft::Net::Remote::DataStream;
using namespace Microsoft::Net::Remote::Service::Reactors;

DataStreamReader::DataStreamReader(DataStreamUploadResult* result) :
    m_result(result)
{
    logging::FunctionTracer traceMe(plog::Severity::debug);
    StartRead(&m_data);
}

void
DataStreamReader::OnReadDone(bool isOk)
{
    logging::FunctionTracer traceMe(plog::Severity::debug);

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
    logging::FunctionTracer traceMe(plog::Severity::debug);

    m_result->set_numberofdatablocksreceived(m_numberOfDataBlocksReceived);
    m_readStatus.set_code(DataStreamOperationStatusCode::DataStreamOperationStatusCodeCanceled);
    m_readStatus.set_message("RPC canceled");
    *m_result->mutable_status() = std::move(m_readStatus);
    Finish(grpc::Status::CANCELLED);
}

void
DataStreamReader::OnDone()
{
    logging::FunctionTracer traceMe(plog::Severity::debug);
    delete this;
}

DataStreamWriter::DataStreamWriter(const DataStreamDownloadRequest* request)
{
    logging::FunctionTracer traceMe(plog::Severity::debug);
    m_dataStreamProperties = request->properties();

    switch (m_dataStreamProperties.type()) {
    case DataStreamType::DataStreamTypeFixed: {
        if (m_dataStreamProperties.Properties_case() == DataStreamProperties::kFixed) {
            m_numberOfDataBlocksToStream = m_dataStreamProperties.fixed().numberofdatablockstostream();
        } else {
            HandleFailure("Invalid properties for this streaming type. Expected Fixed for DataStreamTypeFixed");
            return;
        }

        break;
    }
    case DataStreamType::DataStreamTypeContinuous: {
        if (m_dataStreamProperties.Properties_case() == DataStreamProperties::kContinuous) {
            m_numberOfDataBlocksToStream = 0;
        } else {
            HandleFailure("Invalid properties for this streaming type. Expected Continuous for DataStreamTypeContinuous");
            return;
        }

        break;
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
    logging::FunctionTracer traceMe(plog::Severity::debug);

    // Client may have canceled the RPC, so check for cancelation to prevent writing more data
    // when we shouldn't.
    if (m_isCanceled.load(std::memory_order_relaxed)) {
        LOGD << "RPC canceled, returning early";
        return;
    }

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
    logging::FunctionTracer traceMe(plog::Severity::debug);

    // The RPC is canceled by the client, so call Finish to complete it from the server perspective.
    bool isCanceledExpected{ false };
    if (m_isCanceled.compare_exchange_strong(isCanceledExpected, true, std::memory_order_relaxed, std::memory_order_relaxed)) {
        Finish(grpc::Status::CANCELLED);
    }
}

void
DataStreamWriter::OnDone()
{
    logging::FunctionTracer traceMe(plog::Severity::debug);
    delete this;
}

void
DataStreamWriter::NextWrite()
{
    logging::FunctionTracer traceMe(plog::Severity::debug);

    // Client may have canceled the RPC, so check for cancelation to prevent writing more data
    // when we shouldn't.
    if (m_isCanceled.load(std::memory_order_relaxed)) {
        LOGD << "RPC canceled, aborting write";
        return;
    }

    if (m_dataStreamProperties.type() == DataStreamType::DataStreamTypeContinuous ||
        (m_dataStreamProperties.type() == DataStreamType::DataStreamTypeFixed && m_numberOfDataBlocksToStream > 0)) {
        // Check the requested data streaming pattern.
        const auto pattern = m_dataStreamProperties.pattern();

        switch (pattern) {
        // Generate data with the constant default size and write to the client at a constant speed.
        case DataStreamPattern::DataStreamPatternConstant: {
            // Create data to write to the client.
            const auto data = m_dataGenerator.GenerateRandomData();
            m_numberOfDataBlocksWritten++;

            // Write data to the client.
            m_data.set_data(data);
            m_data.set_sequencenumber(m_numberOfDataBlocksWritten);
            *m_data.mutable_status() = m_writeStatus;
            StartWrite(&m_data);

            break;
        }
        default:
            HandleFailure(std::format("Unexpected data stream pattern {}", magic_enum::enum_name(pattern)));
        };
    } else {
        // No more data to write.
        Finish(::grpc::Status::OK);
    }
}

void
DataStreamWriter::HandleFailure(const std::string& errorMessage)
{
    logging::FunctionTracer traceMe(plog::Severity::debug);

    m_writeStatus.set_code(DataStreamOperationStatusCode::DataStreamOperationStatusCodeFailed);
    m_writeStatus.set_message(errorMessage);
    *m_data.mutable_status() = m_writeStatus;

    // Write a final message to the client. The OnWriteDone() callback will check for the
    // DataStreamOperationStatusCodeFailed status code set here to know to complete the RPC.
    StartWrite(&m_data);
}
