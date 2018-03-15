# ESP8266-Iperf-Test
A simple throughput test for ESP8266 based on iperf

Only support iperf and iperf2, iperf3 is not applicable as it relies on special control packets in the test.

* Hardware:
  
  ESP8266 Launcher Kit

* Software:
  * Platform: lubuntu 14.04 shipped with xtensa toolchain
  * SDK: [ESP8266_RTOS_SDK v2.2](https://github.com/espressif/ESP8266_RTOS_SDK)


## Usage:

Put the folder under the SDK ```examples``` folder, modify the IP address and port information in the user/user_main.c accordingly.
```bash
 $./gen_misc.sh
```
## Test Procedure:

Plug a USB wifi dongle in the PC. Run it in the station mode and connect it to ESP8266 after booting up. Test will start automatically when link is ready.

1. UDP RX

   Enable the macro UDP_TEST and UDP_RX in user/user_main.c

   PC: iperf -c <DUT IP> -t 10 -i 1 -u -b 100M

2. UDP TX

   Enable the macro UDP_TEST, disable UDP_RX in user/user_main.c

   PC: iperf -s -i 1 -u

3. TCP RX

   Enable the macro TCP_TEST and TCP_RX in user/user_main.c

   PC: iperf -c <DUT IP> -t 10 -i 1 -b 100M

4. TCP TX

   Enable the macro TCP_TEST, disable TCP_RX in user/user_main.c

   PC: iperf -s -i 1

## Statistics:

* SoftAP mode

  | Test Type | Throughput | Comment |
  | ---: | --- | --- |
  | UDP RX | 29.9 Mbps | 
  | UDP TX | 15.2 Mbps | buffer size 1460 |
  | TCP RX | 16.9 Mbps | 
  | TCP TX | 10.2 Mbps | buffer size 1440*2 * |

  _* Due to the programming model limit in the espconn, we have to send the next TCP data in the TCP sent callback which is triggered by a TCP ACK, so we send out two packets in a row to avoid Delayed TCP ACK in the receiving end, otherwise TP drops significantly_ 
