
using Grpc.Net.Client;
using Microsoft.Wifi.Remote.Service;

namespace Microsoft.Wifi.Remote.Client.Test
{
    [TestClass]
    public class UnitTestWifiRemoteClient
    {
        private static readonly string AddressHttp = "http://localhost:5047";
        private static readonly string AddressHttps = "https://localhost:7073";

        [TestMethod]
        public void TestSendGetWifiApControl()
        {
            using var channel = GrpcChannel.ForAddress(AddressHttp);

            var client = new WifiRemote.WifiRemoteClient(channel);
            var reply = client.GetWifiApControl(new Request {
                Id = "TestSendGetWifiApControl1",
                RequestType = Request.Types.Type.Basic,
                Payload = Google.Protobuf.ByteString.CopyFromUtf8("Ping"),
            });

            Assert.IsNotNull(reply);
            Assert.IsNotNull(reply.Payload);
            Assert.AreEqual(reply.Payload.ToStringUtf8(), "Pong");
        }
    }
}
