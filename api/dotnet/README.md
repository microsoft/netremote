# Microsoft.Net.Remote.Client

A .NET client library for NetRemote, providing gRPC service bindings and Protocol Buffer definitions for remote network management operations.

## Overview

This package provides:

- **gRPC Client Stubs**: Pre-generated client stubs for all NetRemote services
- **Protocol Buffer Definitions**: Message types for network, Wi-Fi, and RF attenuator operations
- **Proto Files**: Raw `.proto` files included for use with other languages or custom code generation

## Installation

```bash
dotnet add package Microsoft.Net.Remote.Client
```

## Quick Start

```csharp
using Grpc.Net.Client;
using Microsoft.Net.Remote.Service;

// Create a channel to the NetRemote server
using var channel = GrpcChannel.ForAddress("http://localhost:5047");

// Create the client
var client = new NetRemote.NetRemoteClient(channel);

// Enumerate network interfaces
var response = await client.NetworkInterfacesEnumerateAsync(
    new Microsoft.Net.Remote.Network.NetworkEnumerateInterfacesRequest());

foreach (var iface in response.Interfaces)
{
    Console.WriteLine($"Interface: {iface.Id} ({iface.Kind})");
}
```

## Services

The package includes clients for the following services:

### NetRemote Service
Core network management operations including:
- Network interface enumeration
- Wi-Fi access point management (enable/disable, configuration)
- Wi-Fi authentication settings (including 802.1X)

### NetRemote Data Streaming Service
Real-time data streaming capabilities for network monitoring and diagnostics.

### NetRemote RF Attenuator Service
RF attenuator control for wireless testing scenarios.

## Proto Files

The raw `.proto` files are included in the package under the `Protos` content folder. These can be used to generate bindings for other languages or for custom code generation scenarios.

## Requirements

- .NET Core 3.0+ / .NET 5+ / .NET Standard 2.1 compatible runtime
- A running NetRemote server instance

## License

This project is licensed under the MIT License - see the [LICENSE](https://github.com/microsoft/netremote/blob/develop/LICENSE) file for details.

## Links

- [GitHub Repository](https://github.com/microsoft/netremote)
- [Documentation](https://github.com/microsoft/netremote#readme)
