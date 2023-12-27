
using Grpc.Net.Client;
using Microsoft.Net.Remote.Wifi;
using Microsoft.Net.Remote.Service;

namespace Microsoft.Net.Remote.Client.Test
{
    [TestClass]
    public class UnitTestNetRemoteClient
    {
        private static readonly string AddressHttp = "http://localhost:5047";
        private static readonly string AddressHttps = "https://localhost:7073";

        internal enum ConnectionType
        {
            Http,
            Https,
        }

        internal static string ConnectionAddress(ConnectionType connectionType) => connectionType switch
        {
            ConnectionType.Http  => AddressHttp,
            ConnectionType.Https => AddressHttps,
                               _ => throw new NotImplementedException(),
        };

        internal readonly record struct GrpcConnection(GrpcChannel Channel, NetRemote.NetRemoteClient Client);

        internal static GrpcConnection CreateConnection(ConnectionType connectionType)
        {
            var address = ConnectionAddress(connectionType);
            var channel = GrpcChannel.ForAddress(address);
            var client = new NetRemote.NetRemoteClient(channel);

            return new GrpcConnection(channel, client);
        }
    }
}
