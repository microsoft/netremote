using Grpc.Core;
using Microsoft.Wifi.Remote.Service;
using System.Text;

using grpc = global::Grpc.Core;

namespace Microsoft.Wifi.Remote.Service
{
    public class WifiRemoteService : WifiRemote.WifiRemoteBase
    {
        private readonly ILogger<WifiRemoteService> _logger;

        public WifiRemoteService(ILogger<WifiRemoteService> logger)
        {
            _logger = logger;
        }

        public override Task<Microsoft.Wifi.Remote.Response> GetWifiApControl(Microsoft.Wifi.Remote.Request request, grpc::ServerCallContext context)
        {
            return Task.FromResult(new Microsoft.Wifi.Remote.Response
            {
                RequestId = request.Id,
                Payload = Google.Protobuf.ByteString.CopyFromUtf8("Pong")
            });
        }
    }
}
