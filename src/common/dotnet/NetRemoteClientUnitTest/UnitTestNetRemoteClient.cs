
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

        [TestMethod]
        public void TestWifiConfigureAccessPoint()
        {
            var connection = CreateConnection(ConnectionType.Http);
            using var channel = connection.Channel;
 
            foreach (var band in Enum.GetValues(typeof(Net.Wifi.RadioBand)).Cast<Net.Wifi.RadioBand>())
            {
                foreach (var phyType in Enum.GetValues(typeof(Net.Wifi.Dot11PhyType)).Cast<Net.Wifi.Dot11PhyType>())
                {
                    var request = new WifiAccessPointApplyConfigurationRequest
                    {
                        AccessPointId = string.Format("TestWifiConfigureAccessPoint{0}", band),
                        DefaultBand = band,
                    };

                    request.Configurations.Add((int)phyType, new Net.Wifi.AccessPointConfiguration { PhyType = phyType });
                    var result = connection.Client.WifiConfigureAccessPoint(request);

                    Assert.IsNotNull(result);
                    Assert.IsTrue(result.Succeeded);
                    Assert.AreEqual(request.AccessPointId, result.AccessPointId);
                }
            }
        }
    }
}
