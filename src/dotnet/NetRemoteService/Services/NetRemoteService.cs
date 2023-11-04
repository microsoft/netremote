using Grpc.Core;
using Microsoft.Net.Wifi.Remote.Service;
using System.Text;

using grpc = global::Grpc.Core;

namespace Microsoft.Net.Wifi.Remote.Service
{
    public class NetRemoteService : NetRemote.NetRemoteBase
    {
        private readonly ILogger<NetRemoteService> _logger;

        public NetRemoteService(ILogger<NetRemoteService> logger)
        {
            _logger = logger;
        }

        public override Task<Microsoft.Net.Wifi.Remote.Response> GetWifiApControl(Microsoft.Net.Wifi.Remote.Request request, grpc::ServerCallContext context)
        {
            return Task.FromResult(new Microsoft.Net.Wifi.Remote.Response
            {
                RequestId = request.Id,
                Payload = Google.Protobuf.ByteString.CopyFromUtf8("Pong")
            });
        }
    }
}
