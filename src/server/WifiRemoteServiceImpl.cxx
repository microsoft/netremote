
#include "WifiRemoteServiceImpl.hxx"

using namespace Microsoft::Wifi::Remote::Service;

::grpc::Status WifiRemoteServiceImpl::GetWifiApControl([[maybe_unused]] ::grpc::ServerContext* context, [[maybe_unused]] const ::Microsoft::Wifi::Remote::Request* request, [[maybe_unused]] ::Microsoft::Wifi::Remote::Response* response)
{
    return grpc::Status::OK;
}
