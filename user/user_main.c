/*
 * ESPRSSIF MIT License
 *
 * Copyright (c) 2017 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on ESPRESSIF SYSTEMS ESP8266 only, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include "esp_common.h"
#include "user_config.h"
#include "espconn.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

//#define TCP_TEST
#define UDP_TEST

#ifdef TCP_TEST
//#define TCP_RX
#endif

#ifdef UDP_TEST
//#define UDP_RX
#endif

static struct espconn user_espconn;
static os_timer_t test_timer;
//static char buf[1440];  // magic number, traffic is slow with buf size below this
static char buf[1460];
static unsigned int sum = 0;
static unsigned int p_sum = 0;
static const char remote_ip[] = {192,168,4,2};

/******************************************************************************
 * FunctionName : user_rf_cal_sector_set
 * Description  : SDK just reversed 4 sectors, used for rf init data and paramters.
 *                We add this function to force users to set rf cal sector, since
 *                we don't know which sector is free in user's application.
 *                sector map for last several sectors : ABCCC
 *                A : rf cal
 *                B : rf init data
 *                C : sdk parameters
 * Parameters   : none
 * Returns      : rf cal sector
*******************************************************************************/
uint32 user_rf_cal_sector_set(void)
{
    flash_size_map size_map = system_get_flash_size_map();
    uint32 rf_cal_sec = 0;

    switch (size_map) {
        case FLASH_SIZE_4M_MAP_256_256:
            rf_cal_sec = 128 - 5;
            break;

        case FLASH_SIZE_8M_MAP_512_512:
            rf_cal_sec = 256 - 5;
            break;

        case FLASH_SIZE_16M_MAP_512_512:
        case FLASH_SIZE_16M_MAP_1024_1024:
            rf_cal_sec = 512 - 5;
            break;

        case FLASH_SIZE_32M_MAP_512_512:
        case FLASH_SIZE_32M_MAP_1024_1024:
            rf_cal_sec = 1024 - 5;
            break;
        default:
            rf_cal_sec = 0;
            break;
    }

    return rf_cal_sec;
}

void monitor_rx()
{
        printf("Received bytes %d\n", sum - p_sum);
        os_timer_disarm(&test_timer);
        os_timer_setfn(&test_timer, monitor_rx, NULL);
        os_timer_arm(&test_timer, 1000, 0);
        p_sum = sum;
}

/************************ TCP **********************/
void user_tcp_send(void *data)
{
        printf("sending data...\r\n");

        //while (1) {
                espconn_send(&user_espconn, buf, sizeof(buf));
        //}
}

void user_tcp_sent_cb (void *arg)
{
        espconn_send(&user_espconn, buf, sizeof(buf));
}

void user_tcp_recv_cb (void *arg,
                char *pdata,
                unsigned short len)
{
        sum += len;
}

void user_tcp_discon_cb (void *arg)
{
        printf("user_tcp_discon_cb !!! \r\n");
}

void user_tcp_recon_cb (void *arg, sint8 err)
{
        printf("user_tcp_recon_cb !!! \r\n");
}

void user_tcp_connect_cb(void *arg)
{
        struct espconn *pespconn = arg;

        printf("Connected to server...\r\n");

        espconn_regist_recvcb(pespconn, user_tcp_recv_cb);
        espconn_regist_sentcb(pespconn, user_tcp_sent_cb);
        espconn_regist_disconcb(pespconn, user_tcp_discon_cb);

        user_tcp_send(NULL);
}

void user_tcp_listen_cb(void *arg)
{
        struct espconn *pesp_conn = arg;
        printf("user_tcp_listen_cb !!! \r\n");

        espconn_regist_recvcb(&user_espconn, user_tcp_recv_cb);
        espconn_regist_sentcb(&user_espconn, user_tcp_sent_cb);
        espconn_regist_disconcb(&user_espconn, user_tcp_discon_cb);
        espconn_regist_reconcb(pesp_conn, user_tcp_recon_cb);
        monitor_rx();
}

void set_up_tcp ()
{
        user_espconn.type = ESPCONN_TCP;
        user_espconn.state = ESPCONN_NONE;
        user_espconn.proto.tcp = (esp_tcp*)zalloc(sizeof(esp_tcp));
        user_espconn.proto.tcp->local_port = 2525;
        espconn_set_opt(&user_espconn, ESPCONN_NODELAY);

#ifdef TCP_RX
        espconn_regist_connectcb(&user_espconn, user_tcp_listen_cb);

        sint8 ret = espconn_accept(&user_espconn);

        printf("espconn_accept [%d] !!! \r\n", ret);
#else
        const char esp_tcp_server_ip[4] = {192, 168, 4, 2}; // remote IP of TCP server
        memcpy(user_espconn.proto.tcp->remote_ip, esp_tcp_server_ip, 4);

        user_espconn.proto.tcp->remote_port = 5001;

        espconn_regist_connectcb(&user_espconn, user_tcp_connect_cb); // register connect callback
        espconn_regist_reconcb(&user_espconn, user_tcp_recon_cb); // register reconnect callback as error handler
        espconn_connect(&user_espconn);
#endif
        return;
}

/************************ UDP **********************/
void ICACHE_FLASH_ATTR user_udp_send(void *data)
{
        sint8 ret;
        printf("sending data...\r\n");

        while (1) {
                ret = espconn_send(&user_espconn, buf, sizeof(buf));
                //if (ret) os_delay_us(5000);
        }
}

void ICACHE_FLASH_ATTR user_udp_sent_cb(void *arg)
{
        //espconn_send(&user_espconn, buf, sizeof(buf));
}

void ICACHE_FLASH_ATTR user_udp_recv_cb(void *arg,
                        char *pdata,
                        unsigned short len)
{
        sum += len;
}

void set_up_udp ()
{
        printf("Local port 5001, remote port 5001\r\n");
        user_espconn.type = ESPCONN_UDP;
        user_espconn.proto.udp = (esp_udp*)zalloc(sizeof(esp_udp));
        user_espconn.proto.udp->local_port = 5001;
        user_espconn.proto.udp->remote_port = 5001;

        const char udp_remote_ip[] = {192,168,4,2}; //用于存放远程IP地址
        memcpy(&user_espconn.proto.udp->remote_ip, udp_remote_ip, sizeof(udp_remote_ip));

        espconn_regist_recvcb(&user_espconn, user_udp_recv_cb); //接收回调函数
        espconn_regist_sentcb(&user_espconn, user_udp_sent_cb); //发送回调函数
        espconn_create(&user_espconn); //创建UDP连接

#ifdef UDP_RX
        monitor_rx();
#else
        user_udp_send(NULL);
#endif

        return;
}

/************************ WiFi **********************/
LOCAL void ICACHE_FLASH_ATTR on_client_connect()
{
    printf("freeheap %d\n", system_get_free_heap_size());
    printf("A STA connected\n");
#ifdef UDP_TEST
    set_up_udp();
#endif
#ifdef TCP_TEST
    set_up_tcp();
#endif
}

void print_test_info()
{
    printf(
#ifdef UDP_TEST
    ">> UDP "
#ifdef UDP_RX
           "RX"
#else
           "TX"
#endif
#else
    "TCP "
#ifdef TCP_RX
           "RX"
#else
           "TX"
#endif
#endif
           " test <<\r\n");
}

void user_init(void)
{
    set_on_client_connect(on_client_connect);
    init_esp_wifi();
    stop_wifi_station();
    print_test_info();
    start_wifi_ap("ESP_TEST1", "12345678");

}
