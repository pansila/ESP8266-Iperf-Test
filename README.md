# ESP8266-Iperf-Test
A simple throughput test for ESP8266 based on iperf

Only support iperf and iperf2, iperf3 is not applicable as it relies on special control packets in the test.

* Hardware:
  
  ESP8266 Launcher Kit

* Software:
  * Platform: lubuntu 14.04 shipped with xtensa toolchain
  * SDK: [ESP8266_RTOS_SDK v2.2](https://github.com/espressif/ESP8266_RTOS_SDK)


## Usage:

Put the folder under the SDK ```examples``` folder
```bash
 $./gen_misc.sh
```
## Test Procedure:

Plug a USB wifi dongle in the PC. Run it in the station mode and connect it to ESP8266 after booting up. Test will start automatically when link is ready.

1. UDP RX

   Enable the macro UDP_TEST and UDP_RX in user/user_main.c

   PC: iperf -c <ESP8266 IP> -t 10 -i 1 -u -b 100M
2. UDP TX

    iperf -s -i 1 -u

    Enable the macro UDP_TEST, disable UDP_RX in user/user_main.c
3. TCP RX

   PC: iperf -c <ESP8266 IP> -t 10 -i 1 -b 100M

   Enable the macro TCP_TEST and TCP_RX in user/user_main.c
4. TCP TX

    iperf -s -i 1

    Enable the macro TCP_TEST, disable TCP_RX in user/user_main.c

## Statistics:

  | Test Type | Throughput |
  | ---: | --- |
  | UDP UP (PC -> DUT) | 29.9 Mbps |
  | UDP DOWN (DUT -> PC) | 15.2 Mbps |
  | TCP UP (PC -> DUT) | 15.6 Mbps |
  | TCP DOWN (DUT -> PC) | 6.7 Mbps |
