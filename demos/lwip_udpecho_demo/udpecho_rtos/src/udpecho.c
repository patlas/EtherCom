/*
 * Copyright (c) 2013 - 2014, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
 
#include "lwip/opt.h"

#if LWIP_NETCONN

#include <stdio.h>
#include "lwip/udp.h"
#include "lwip/debug.h"
#include "netif/etharp.h"
#include "lwip/init.h"
#include "lwip/tcpip.h"

#include "fsl_clock_manager.h"
#include "fsl_uart_driver.h"
#include "fsl_device_registers.h"
#include "fsl_port_hal.h"
#include "fsl_sim_hal.h"
#include "fsl_os_abstraction.h"
#include "ethernetif.h"
#include "board.h"

uint32_t portBaseAddr[] = PORT_BASE_ADDRS;
uint32_t simBaseAddr[] = SIM_BASE_ADDRS;
struct netif fsl_netif0;

#ifndef UDPECHO_STACKSIZE
#define UDPECHO_STACKSIZE        3000
#endif

#ifndef UDPECHO_PRIORITY
#define UDPECHO_PRIORITY          3
#endif

#ifndef UDPECHO_DBG
#define UDPECHO_DBG     LWIP_DBG_ON
#endif
#if defined(FSL_RTOS_MQX)
void Main_Task(uint32_t param);
TASK_TEMPLATE_STRUCT  MQX_template_list[] =
    {
   { 1L,     Main_Task,      3000L,  MQX_MAIN_TASK_PRIORITY, "Main",      MQX_AUTO_START_TASK},
   { 0L,     0L,             0L,    0L, 0L,          0L }
};
#endif
static void udpecho_thread(void *arg)
{
  static struct netconn *conn;
  static struct netbuf *buf;
  char buffer[100];
  err_t err;
  LWIP_UNUSED_ARG(arg);
  netif_set_up(&fsl_netif0);
  conn = netconn_new(NETCONN_UDP);
  LWIP_ASSERT("con != NULL", conn != NULL);
  netconn_bind(conn, NULL, 7);

  while (1) {
    err = netconn_recv(conn, &buf);
    if (err == ERR_OK) {
      if(netbuf_copy(buf, buffer, buf->p->tot_len) != buf->p->tot_len) {
        LWIP_DEBUGF(UDPECHO_DBG, ("netbuf_copy failed\r\n"));
      } else {
        buffer[buf->p->tot_len] = '\0';
        err = netconn_send(conn, buf);
        if(err != ERR_OK) {
          LWIP_DEBUGF(UDPECHO_DBG, ("netconn_send failed: %d\r\n", (int)err));
        } else {
          LWIP_DEBUGF(UDPECHO_DBG, ("got %s\r\n", buffer));
        }
      }
      netbuf_delete(buf);
    }
  }
}

void udp_echo_init(void)
{

  sys_thread_new("udpecho_thread", udpecho_thread, NULL, UDPECHO_STACKSIZE, UDPECHO_PRIORITY);
  OSA_Start();	
}
static void init_hardware(void)
{
    /* Open uart module for debug */
    hardware_init();

    dbg_uart_init();
    
    /* Open ENET clock gate*/
    CLOCK_SYS_EnableEnetClock(0);
    /* Select PTP timer outclk*/
    CLOCK_SYS_SetSource(kClockTimeSrc, 2);

    /* Disable the mpu*/
    BW_MPU_CESR_VLD(MPU_BASE, 0);
}
#if defined(FSL_RTOS_MQX)
void Main_Task(uint32_t param)
#else
int main(void)
#endif
{
  ip_addr_t fsl_netif0_ipaddr, fsl_netif0_netmask, fsl_netif0_gw;
  init_hardware();
  OSA_Init();
  printf("TCP/IP initializing...\r\n");  
  tcpip_init(NULL,NULL);
  printf("TCP/IP initialized.\r\n");
  IP4_ADDR(&fsl_netif0_ipaddr, 192,168,2,102);
  IP4_ADDR(&fsl_netif0_netmask, 255,255,255,0);
  IP4_ADDR(&fsl_netif0_gw, 192,168,2,100);
  netif_add(&fsl_netif0, &fsl_netif0_ipaddr, &fsl_netif0_netmask, &fsl_netif0_gw, NULL, ethernetif_init, tcpip_input);
  netif_set_default(&fsl_netif0);  
  udp_echo_init();
  for(;;);
}
#endif
