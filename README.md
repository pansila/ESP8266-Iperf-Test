# ESP8266-Iperf-Test
A simple throughput test for ESP8266 based on iperf

Only support iperf and iperf2, iperf3 is not applicable as it relies on special control packets in the test.

* Hardware:
  
  ESP8266 Launcher Kit

* Software:
  * Platform: lubuntu 14.04 shipped with xtensa toolchain
  * SDK: [ESP8266_RTOS_SDK v2.2](https://github.com/espressif/ESP8266_RTOS_SDK)
  * SDK: [ESP8266_RTOS_SDK v1.4.x](https://github.com/espressif/ESP8266_RTOS_SDK/tree/1.4.x)


## Usage:

Put the folder under the SDK ```examples``` folder, modify the IP address and port information in the user/user_main.c accordingly. Specify the SDK path in the ```gen_misc.sh```. Build the image:
```bash
 $./gen_misc.sh
```
## Test Procedure:

Plug a USB wifi dongle in the PC. Run it in the station mode and connect it to ESP8266 after booting up. Test will start automatically when link is ready.

1. UDP RX

   Enable the macro UDP_TEST and UDP_RX in user/user_main.c

   PC: iperf -c <DUT IP> -t 10 -i 1 -u -b 40M

   _Note: Too large bumping rate over the potential max bandwidth reduces the TP number_

2. UDP TX

   Enable the macro UDP_TEST, disable UDP_RX in user/user_main.c

   PC: iperf -s -i 1 -u

3. TCP RX

   Enable the macro TCP_TEST and TCP_RX in user/user_main.c

   PC: iperf -c <DUT IP> -t 10 -i 1 -b 40M

4. TCP TX

   Enable the macro TCP_TEST, disable TCP_RX in user/user_main.c

   PC: iperf -s -i 1

## Statistics:

  * RTOS SDK 2.2

    | Test Case | SoftAP (Open) | SoftAP (WPA2) | Station (Open) | Station (WPA2) | Comment |
    | --- | --- | --- | --- | --- | --- |
    | UDP RX | 14.9 Mbps | 14.5 Mbps | 10.2 Mbps | 10.5 Mbps |
    | UDP TX * | 11.1 Mpbs | 11.1 Mbps | N/A ** | N/A | buffer size 1460 *** |
    | TCP RX | 7.2 Mpbs | 6.5 Mbps | 5.1 Mbps | 5.9 Mbps |
    | TCP TX | 7.2 Mpbs | 6.7 Mbps | 4.3 Mbps | 4.5 Mbps | buffer size 1440*2 **** |

    _* AMPDU is not applied and OFDM date rate (11g) instead of HT rate (11n) is used when sending frames in my case, that's probably why the TP reduces a lot compared to RX case._

    _** Crash happens repeatedly, no result._

    _*** Buffer size has been carefully chosen to work with the MSS size of TCP/UDP to reduce the TP penalty by the ineffcient fragmentation._

    _**** Due to the programming model limit in the espconn, we have to send the next TCP data in the TCP sent callback which is triggered by a TCP ACK, so we send out two packets in a row to avoid Delayed TCP ACK in the receiving end, otherwise TP drops significantly._

  * RTOS SDK 1.4.x
  
    | Test Case | SoftAP (Open) | SoftAP (WPA2) | Station (Open) | Station (WPA2) | Comment |
    | --- | --- | --- | --- | --- | --- |
    | UDP RX | 27.6 Mbps | 26.5 Mbps | 28.3 Mbps | 27.3 Mbps |
    | UDP TX | 17.1 Mpbs | N/A | N/A | N/A | buffer size 1460 |
    | TCP RX | 16.5 Mpbs | 15.5 Mbps | 11.5 Mbps | 11.2 Mbps |
    | TCP TX | 11.2 Mpbs | 10.1 Mbps | 5.8 Mbps | 5.2 Mbps | buffer size 1440*2 |

    Throughtput of RTOS SDK 1.4.x is better all around than RTOS SDK 2.2
