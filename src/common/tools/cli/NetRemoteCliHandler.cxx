
#include <microsoft/net/remote/INetRemoteCliHandlerOperations.hxx>
#include <microsoft/net/remote/NetRemoteCli.hxx>
#include <microsoft/net/remote/NetRemoteCliHandler.hxx>
#include <plog/Log.h>

using namespace Microsoft::Net::Remote;

NetRemoteCliHandler::NetRemoteCliHandler(std::shared_ptr<INetRemoteCliHandlerOperations> operations) :
    m_operations(std::move(operations))
{
}

void
NetRemoteCliHandler::SetParent(std::weak_ptr<NetRemoteCli> parent)
{
    m_parent = parent;
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
