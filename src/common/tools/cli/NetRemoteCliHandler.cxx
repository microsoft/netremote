
#include <memory>
#include <utility>

#include <microsoft/net/remote/INetRemoteCliHandlerOperations.hxx>
#include <microsoft/net/remote/NetRemoteCli.hxx>
#include <microsoft/net/remote/NetRemoteCliHandler.hxx>
#include <microsoft/net/remote/NetRemoteServerConnection.hxx>
#include <plog/Log.h>

using namespace Microsoft::Net::Remote;

NetRemoteCliHandler::NetRemoteCliHandler(std::unique_ptr<INetRemoteCliHandlerOperationsFactory> operationsFactory) :
    m_operationsFactory(std::move(operationsFactory))
{
}

void
NetRemoteCliHandler::SetParent(std::weak_ptr<NetRemoteCli> parent)
{
    m_parent = std::move(parent);
}

void
NetRemoteCliHandler::SetConnection(std::shared_ptr<NetRemoteServerConnection> connection)
{
    m_connection = std::move(connection);
    m_operations = m_operationsFactory->Create(m_connection);
}

std::shared_ptr<NetRemoteCli>
NetRemoteCliHandler::GetParentStrongRef() const
{
    return m_parent.lock();
}

void
NetRemoteCliHandler::HandleCommandWifiEnumerateAccessPoints()
{
    if (!m_operations) {
        LOGE << "No operations instance available to handle command";
        return;
    }

    auto parentStrong{ GetParentStrongRef() };
    if (parentStrong == nullptr) {
        LOGW << "Parent cli object is no longer valid, aborting command";
        return;
    }

    LOGD << "Executing command WifiEnumerateAccessPoints";
    m_operations->WifiEnumerateAccessPoints();
}
