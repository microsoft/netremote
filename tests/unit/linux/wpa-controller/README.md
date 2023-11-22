
# WPA Controller Tests

## Wi-Fi Drivers and WPA Daemons

Some of the tests that exercise wpa daemon (hostapd, wpa_supplicant) functionality require a wlan device driver and a running instance of the daemon. To satisfy these requirements, the mac80211_hwsim driver is used to create a virtualized wlan device that the wpa daemon can then control. Helpers are provided for managing the virtualized wlan device(s) with [WifiVirtualDeviceManager](./detail/WifiVirtualDeviceManager.hxx) and for managing the wpa daemons with [WpaDaemonManager](./detail/WpaDaemonManager.hxx) and [WpaDaemonInstance](./detail/WpaDaemonInstance.hxx).

Presently, the tests use [event listeners](https://github.com/catchorg/Catch2/blob/devel/docs/event-listeners.md) from the [Catch2](https://github.com/catchorg/Catch2) unit test framework instead of traditional, class-based setup/teardown fixtures. The event listeners have access to test case information such as the test case name and their tags. The [WpaDaemonCatch2EventListener](./detail/WpaDaemonCatch2EventListener.hxx) was developed to detect when a test case requires either of the daemons, and automatically creates a wlan driver and starts the required daemon. Similarly, it handles removing the driver instances and stopping the daemons on test case completion.

To trigger this behavior, the test case must include a tag that corresponds to the name of a [`WpaType`](../../../../linux/wpa-controller/include/Wpa/WpaCore.hxx) enumeration value (a case insensitive comparison is done). For example, if the hostapd daemon is needed for the test, then a tag corresponding to `WpaType::Hostapd` is needed in the test case definition, for example '**hostapd**' as shown below:

```C++
TEST_CASE("Send command: GetStatus()", "[wpa][hostapd][client][remote]")
```

Only the first `WpaType` will be used. Failure to add the appropriate tag will cause the test to fail since no daemon will be running and thus, no connection to it will be possible.

### Future Work

This method will be updated in the future, with some possible improvements including:

1. Allow multiple wpa daemon types.
2. Allow a more generic daemon specification (eg. `[wpaDaemons=hostapd,wpaSupplicant]`)
3. Replacement of event-based fixtures with class-based fixtures.
