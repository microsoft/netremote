
#include <memory>

#include <Wpa/IWpaEventListener.hxx>
#include <Wpa/WpaEventListenerProxy.hxx>
#include <notstd/Memory.hxx>

using namespace Wpa;

WpaEventListenerProxy::WpaEventListenerProxy(IWpaEventListener &wpaEventListenerProxy) :
    m_wpaEventListenerProxy(wpaEventListenerProxy)
{
}

/* static */
std::shared_ptr<WpaEventListenerProxy>
WpaEventListenerProxy::Create(IWpaEventListener &wpaEventListenerProxy)
{
    return std::make_shared<notstd::enable_make_protected<WpaEventListenerProxy>>(wpaEventListenerProxy);
}

void
WpaEventListenerProxy::OnWpaEvent(WpaEventSender *sender, const WpaEventArgs *eventArgs)
{
    m_wpaEventListenerProxy.OnWpaEvent(sender, eventArgs);
}
