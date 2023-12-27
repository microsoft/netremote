
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
    }
}
