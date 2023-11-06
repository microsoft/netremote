using Grpc.Core;
using Microsoft.Net.Remote.Service;
using Microsoft.Net.Remote.Wifi;
using System.Text;

using grpc = global::Grpc.Core;

namespace Microsoft.Net.Remote.Service
{
    public class NetRemoteService : NetRemote.NetRemoteBase
    {
        private readonly ILogger<NetRemoteService> _logger;

        public NetRemoteService(ILogger<NetRemoteService> logger)
        {
            _logger = logger;
        }

        public override Task<Microsoft.Net.Remote.Response> GetWifiApControl(Microsoft.Net.Remote.Request request, grpc::ServerCallContext context)
        {
            return Task.FromResult(new Microsoft.Net.Remote.Response
            {
                RequestId = request.Id,
                Payload = Google.Protobuf.ByteString.CopyFromUtf8("Pong")
            });
        }

        public override Task<Microsoft.Net.Remote.Wifi.WifiConfigureAccessPointResult> WifiConfigureAccessPoint(Microsoft.Net.Remote.Wifi.WifiConfigureAccessPointRequest request, grpc::ServerCallContext context)
        {
            return Task.FromResult(new Microsoft.Net.Remote.Wifi.WifiConfigureAccessPointResult
            {
                Succeeded = true
            });
        }
    }
}
