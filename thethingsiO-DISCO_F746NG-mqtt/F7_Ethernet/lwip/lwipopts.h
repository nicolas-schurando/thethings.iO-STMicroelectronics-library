////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// For STM32F746
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __LWIPOPTS_H__
#define __LWIPOPTS_H__

#include <limits.h>
#include "stdlib.h"
#include "cmsis_os.h"
#include "stm32f7xx_hal.h"


//Debug Options
#define LWIP_DEBUG
#define SYS_DEBUG                   LWIP_DBG_OFF
#define SYSARCH_DEBUG               LWIP_DBG_ON
#define NETIF_DEBUG                 LWIP_DBG_OFF
#define PPP_DEBUG                   LWIP_DBG_OFF
#define IP_DEBUG                    LWIP_DBG_OFF
#define MEM_DEBUG                   LWIP_DBG_OFF
#define MEMP_DEBUG                  LWIP_DBG_OFF
#define PBUF_DEBUG                  LWIP_DBG_OFF
#define API_LIB_DEBUG               LWIP_DBG_OFF
#define API_MSG_DEBUG               LWIP_DBG_OFF
#define TCPIP_DEBUG                 LWIP_DBG_OFF
#define SOCKETS_DEBUG               LWIP_DBG_OFF
#define TCP_DEBUG                   LWIP_DBG_OFF
#define TCP_INPUT_DEBUG             LWIP_DBG_OFF
#define TCP_FR_DEBUG                LWIP_DBG_OFF
#define TCP_RTO_DEBUG               LWIP_DBG_OFF
#define TCP_CWND_DEBUG              LWIP_DBG_OFF
#define TCP_WND_DEBUG               LWIP_DBG_OFF
#define TCP_OUTPUT_DEBUG            LWIP_DBG_OFF
#define TCP_RST_DEBUG               LWIP_DBG_OFF
#define TCP_QLEN_DEBUG              LWIP_DBG_OFF
#define ETHARP_DEBUG                LWIP_DBG_OFF
#define DHCP_DEBUG                  LWIP_DBG_OFF
#define SNMP_MSG_DEBUG              LWIP_DBG_OFF
#define SNMP_MIB_DEBUG              LWIP_DBG_OFF
#define DNS_DEBUG                   LWIP_DBG_OFF

#define SYS_LIGHTWEIGHT_PROT    1
#define ETHARP_TRUST_IP_MAC     0
#define IP_REASSEMBLY           0
#define IP_FRAG                 0
#define ARP_QUEUEING            0
#define ETH_PAD_SIZE            0

#define MTU                         1500             // Maximum transfer unit
#define TCPIP_MBOX_SIZE             8
#define DEFAULT_TCP_RECVMBOX_SIZE   8
#define DEFAULT_UDP_RECVMBOX_SIZE   8
#define DEFAULT_RAW_RECVMBOX_SIZE   8
#define DEFAULT_ACCEPTMBOX_SIZE     8
#define LOCK_RX_THREAD
#define LWIP_TCPIP_CORE_LOCKING         0
#define LWIP_TCPIP_CORE_LOCKING_INPUT   0
#define LWIP_CHECKSUM_ON_COPY           1
#define EMAC_RECIVE_THREAD_STACKSIZE  1536
#define DEFAULT_THREAD_STACKSIZE    512
#define TCPIP_THREAD_STACKSIZE      1536
#define TCPIP_THREAD_PRIO           (osPriorityNormal)

#define LWIP_EVENT_API                  0
#define LWIP_CALLBACK_API               1

#define ERRNO                           1
#define NO_SYS                          0
#define MEM_ALIGNMENT                   4

#define PBUF_POOL_BUFSIZE               1524


 #define MEM_SIZE                        (24*1024)
 #define MEMP_NUM_PBUF                   50
 #define MEMP_NUM_UDP_PCB                10
 #define MEMP_NUM_TCP_PCB                20
 #define MEMP_NUM_TCP_PCB_LISTEN         20
 #define MEMP_NUM_TCP_SEG                12
 #define MEMP_NUM_SYS_TIMEOUT            10
 #define MEMP_NUM_NETBUF                 4
 #define MEMP_NUM_NETCONN                10
 #define PBUF_POOL_SIZE                  10



#define TCP_TTL                         255
#define TCP_QUEUE_OOSEQ                 0
#define TCP_MIN_SEGLEN                  (MTU - 40)
#define TCP_MSS                         (MTU - 40)
#define TCP_SND_BUF                     (4*TCP_MSS)
#define TCP_SND_QUEUELEN                (2*TCP_SND_BUF/TCP_MSS)
#define TCP_WND                         (2 * TCP_MSS)
#define TCP_SNDLOWAT                    (TCP_SND_BUF/2)


#define LWIP_RAW                        0
#define TCPIP_LWIP                      1
#define LWIP_NETIF_API                  1
#define LWIP_DHCP                       1
#define LWIP_TCP                        1
#define LWIP_UDP                        1
#define LWIP_DNS                        1
#define LWIP_ICMP                       1
#define LWIP_IGMP                       1
#define LWIP_ARP                        1
#define LWIP_AUTOIP                     0
#define LWIP_SNMP                       0
#define LWIP_NETBIOS                    1
#define NETBIOS_LWIP_NAME               ("STM32F746")
#define LWIP_NETIF_HOSTNAME             1
#define NETIF_HOSTNAME                  ("STM32F746")
#define LWIP_NETIF_LOOPBACK             1
#define LWIP_HAVE_LOOPIF                1

#define LWIP_NETIF_LINK_CALLBACK        1
#define LWIP_NETIF_STATUS_CALLBACK      1

#define LWIP_SO_RCVBUF                  0

#define LWIP_RAND                       rand

#ifdef DEBUG
#define DHCP_CREATE_RAND_XID            0
#endif

// Keepalive values, compliant with RFC 1122. Don't change this unless you know what you're doing
#define TCP_KEEPIDLE_DEFAULT            10000UL // Default KEEPALIVE timer in milliseconds
#define TCP_KEEPINTVL_DEFAULT           2000UL  // Default Time between KEEPALIVE probes in milliseconds
#define TCP_KEEPCNT_DEFAULT             9U      // Default Counter for KEEPALIVE probes

#define CHECKSUM_BY_HARDWARE
#ifdef CHECKSUM_BY_HARDWARE
  /* CHECKSUM_GEN_IP==0: Generate checksums by hardware for outgoing IP packets.*/
  #define CHECKSUM_GEN_IP                 0
  /* CHECKSUM_GEN_UDP==0: Generate checksums by hardware for outgoing UDP packets.*/
  #define CHECKSUM_GEN_UDP                0
  /* CHECKSUM_GEN_TCP==0: Generate checksums by hardware for outgoing TCP packets.*/
  #define CHECKSUM_GEN_TCP                0
  /* CHECKSUM_CHECK_IP==0: Check checksums by hardware for incoming IP packets.*/
  #define CHECKSUM_CHECK_IP               0
  /* CHECKSUM_CHECK_UDP==0: Check checksums by hardware for incoming UDP packets.*/
  #define CHECKSUM_CHECK_UDP              0
  /* CHECKSUM_CHECK_TCP==0: Check checksums by hardware for incoming TCP packets.*/
  #define CHECKSUM_CHECK_TCP              0
  /* CHECKSUM_GEN_ICMP==1: Generate checksums in software for outgoing ICMP packets.*/
  #define CHECKSUM_GEN_ICMP               0
#else
  /* CHECKSUM_GEN_IP==1: Generate checksums in software for outgoing IP packets.*/
  #define CHECKSUM_GEN_IP                 1
  /* CHECKSUM_GEN_UDP==1: Generate checksums in software for outgoing UDP packets.*/
  #define CHECKSUM_GEN_UDP                1
  /* CHECKSUM_GEN_TCP==1: Generate checksums in software for outgoing TCP packets.*/
  #define CHECKSUM_GEN_TCP                1
  /* CHECKSUM_CHECK_IP==1: Check checksums in software for incoming IP packets.*/
  #define CHECKSUM_CHECK_IP               1
  /* CHECKSUM_CHECK_UDP==1: Check checksums in software for incoming UDP packets.*/
  #define CHECKSUM_CHECK_UDP              1
  /* CHECKSUM_CHECK_TCP==1: Check checksums in software for incoming TCP packets.*/
  #define CHECKSUM_CHECK_TCP              1
  /* CHECKSUM_GEN_ICMP==1: Generate checksums in software for outgoing ICMP packets.*/
  #define CHECKSUM_GEN_ICMP               1
#endif



#endif /* __LWIPOPTS_H__ */
