
#include "WifiApControlServiceImpl.hxx"

using namespace Microsoft::Wifi::Test::ApControl::Service;

::grpc::Status WifiApControlServiceImpl::GetWifiApControl([[maybe_unused]] ::grpc::ServerContext* context, [[maybe_unused]] const ::Microsoft::Wifi::Test::ApControl::Request* request, [[maybe_unused]] ::Microsoft::Wifi::Test::ApControl::Response* response)
{
    return grpc::Status::OK;
}
