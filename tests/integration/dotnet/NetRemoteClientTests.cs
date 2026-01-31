
using Grpc.Net.Client;
using Microsoft.Net.Remote.Wifi;
using Microsoft.Net.Remote.Service;

namespace Microsoft.Net.Remote.Client.IntegrationTests
{
    [TestClass]
    public class NetRemoteClientTests
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

        [TestMethod]
        public void CanCreateGrpcChannel()
        {
            // Verify that a gRPC channel can be created with the expected address
            using var channel = GrpcChannel.ForAddress(AddressHttp);
            Assert.IsNotNull(channel);
        }

        [TestMethod]
        public void CanCreateNetRemoteClient()
        {
            // Verify that a NetRemote client can be instantiated
            using var channel = GrpcChannel.ForAddress(AddressHttp);
            var client = new NetRemote.NetRemoteClient(channel);
            Assert.IsNotNull(client);
        }
    }
}
