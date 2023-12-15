
using Grpc = global::Grpc.Core;

namespace Microsoft.Net.Remote.Service
{
    public class NetRemoteService : NetRemote.NetRemoteBase
    {
        private readonly ILogger<NetRemoteService> _logger;

        public NetRemoteService(ILogger<NetRemoteService> logger)
        {
            _logger = logger;
        }

        public override Task<Microsoft.Net.Remote.Wifi.WifiAccessPointApplyConfigurationResult> WifiConfigureAccessPoint(Microsoft.Net.Remote.Wifi.WifiAccessPointApplyConfigurationRequest request, Grpc::ServerCallContext context)
        {
            return Task.FromResult(new Microsoft.Net.Remote.Wifi.WifiAccessPointApplyConfigurationResult
            {
                AccessPointId = request.AccessPointId,
                Succeeded = true,
            });
        }
    }
}
