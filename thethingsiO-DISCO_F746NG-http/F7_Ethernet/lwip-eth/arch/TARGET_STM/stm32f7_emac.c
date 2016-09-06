
#include "stm32f7xx_hal.h"
#include "lwipopts.h"
#include "lwip/opt.h"
#include "wait_api.h"
//#include "lwip/timers.h"
#include "netif/etharp.h"
#include "lwip/tcpip.h"
#include <string.h>
#include "cmsis_os.h"
#include "mbed_interface.h"


/** @defgroup lwipstm32f7xx_emac_DRIVER	stm32f7 EMAC driver for LWIP
 * @ingroup lwip_emac
 *
 * @{
 */

#define RECV_TASK_PRI           (osPriorityHigh)
#define PHY_TASK_PRI            (osPriorityLow)
#define PHY_TASK_WAIT           (200)

//#define EMAC_RECIVE_WAIT
#define EMAC_R_WAIT 200
//#define EMAC_TRANSMIT_WAIT
#define EMAC_T_WAIT 200



 /* LAN8742A PHY Address*/
#define LAN8742A_PHY_ADDRESS            0x00U


__ALIGN_BEGIN ETH_DMADescTypeDef DMARxDscrTab[ETH_RXBUFNB]  __ALIGN_END; /* Ethernet Rx MA Descriptor */


__ALIGN_BEGIN uint8_t Rx_Buff[ETH_RXBUFNB][ETH_RX_BUF_SIZE]  __ALIGN_END; /* Ethernet Receive Buffer */


__ALIGN_BEGIN ETH_DMADescTypeDef DMATxDscrTab[ETH_TXBUFNB]  __ALIGN_END; /* Ethernet Tx DMA Descriptor */


__ALIGN_BEGIN uint8_t Tx_Buff[ETH_TXBUFNB][ETH_TX_BUF_SIZE]  __ALIGN_END; /* Ethernet Transmit Buffer */

uint8_t macaddress[6];

ETH_HandleTypeDef heth;
uint32_t phy_status = 0;
uint32_t systickval;
uint32_t sysflag=0;
static sys_sem_t rx_ready_sem;    /* receive ready semaphore */
static sys_mutex_t tx_lock_mutex;

/* function */
static void stm32f7_rx_task(void *arg);
static void stm32f7_phy_task(void *arg);
static err_t stm32f7_etharp_output(struct netif *netif, struct pbuf *q, ip_addr_t *ipaddr);
static err_t stm32f7_low_level_output(struct netif *netif, struct pbuf *p);

/**
 * Override HAL Eth Init function
 */
void HAL_ETH_MspInit(ETH_HandleTypeDef* heth)
{
  GPIO_InitTypeDef GPIO_InitStructure;
    if (heth->Instance == ETH) {
#if defined(TARGET_STM32F746ZG)
// Nucleo STM32F746 Board
  /* Enable GPIOs clocks */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();

/* Ethernet pins configuration ************************************************/
  /*
        RMII_REF_CLK ----------------------> PA1
        RMII_MDIO -------------------------> PA2
        RMII_MDC --------------------------> PC1
        RMII_MII_CRS_DV -------------------> PA7
        RMII_MII_RXD0 ---------------------> PC4
        RMII_MII_RXD1 ---------------------> PC5
        RMII_MII_RXER ---------------------> PG2
        RMII_MII_TX_EN --------------------> PG11
        RMII_MII_TXD0 ---------------------> PG13
        RMII_MII_TXD1 ---------------------> PB13
  */

  /* Configure PA1, PA2 and PA7 */
  GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
  GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStructure.Pull = GPIO_NOPULL;
  GPIO_InitStructure.Alternate = GPIO_AF11_ETH;
  GPIO_InitStructure.Pin = GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_7;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* Configure PB13 */
  GPIO_InitStructure.Pin = GPIO_PIN_13;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);

  /* Configure PC1, PC4 and PC5 */
  GPIO_InitStructure.Pin = GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);

  /* Configure PG2, PG11, PG13 and PG14 */
  GPIO_InitStructure.Pin =  GPIO_PIN_2 | GPIO_PIN_11 | GPIO_PIN_13;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStructure);

  /* Enable the Ethernet global Interrupt */
  HAL_NVIC_SetPriority(ETH_IRQn, 0x7, 0);
  HAL_NVIC_EnableIRQ(ETH_IRQn);

  /* Enable ETHERNET clock  */
  __HAL_RCC_ETH_CLK_ENABLE();
#endif
#if defined(TARGET_STM32F746NG)
//Discovery STM32F746 Board
   /* Enable GPIOs clocks */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_ETH_CLK_ENABLE();
/* Ethernet pins configuration ************************************************/
  /*
        RMII_REF_CLK ----------------------> PA1
        RMII_MDIO -------------------------> PA2    HAL_Init();
        RMII_MDC --------------------------> PC1
        RMII_MII_CRS_DV -------------------> PA7
        RMII_MII_RXD0 ---------------------> PC4
        RMII_MII_RXD1 ---------------------> PC5
        RMII_MII_RXER ---------------------> PG2
        RMII_MII_TX_EN --------------------> PG11
        RMII_MII_TXD0 ---------------------> PG13
        RMII_MII_TXD1 ---------------------> PG14
  */

  /* Configure PA1, PA2 and PA7 */
  GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
  GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStructure.Pull = GPIO_NOPULL;
  GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStructure.Alternate = GPIO_AF11_ETH;
  GPIO_InitStructure.Pin = GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_7;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* Configure PC1, PC4 and PC5 */
  GPIO_InitStructure.Pin = GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);

  /* Configure PG2, PG11, PG13 and PG14 */
  GPIO_InitStructure.Pin =  GPIO_PIN_11 | GPIO_PIN_13 | GPIO_PIN_14;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStructure);
  HAL_NVIC_SetPriority(ETH_IRQn, 0x7, 0);
  HAL_NVIC_EnableIRQ(ETH_IRQn);
#endif
    }

}

/**
 * Override HAL Eth DeInit function
 */
void HAL_ETH_MspDeInit(ETH_HandleTypeDef* heth)
{
    if (heth->Instance == ETH) {
#if defined(TARGET_STM32F746ZG)
//Nucleo STM32F746 Board
       __ETH_CLK_DISABLE();
/* Ethernet pins configuration ************************************************/
  /*
        RMII_REF_CLK ----------------------> PA1
        RMII_MDIO -------------------------> PA2
        RMII_MDC --------------------------> PC1
        RMII_MII_CRS_DV -------------------> PA7
        RMII_MII_RXD0 ---------------------> PC4
        RMII_MII_RXD1 ---------------------> PC5
        RMII_MII_RXER ---------------------> PG2
        RMII_MII_TX_EN --------------------> PG11
        RMII_MII_TXD0 ---------------------> PG13
        RMII_MII_TXD1 ---------------------> PB13
  */
        HAL_GPIO_DeInit(GPIOC,  GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5);

        HAL_GPIO_DeInit(GPIOB, GPIO_PIN_13);

        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_7);

        HAL_GPIO_DeInit(GPIOG, GPIO_PIN_2 | GPIO_PIN_11 | GPIO_PIN_13);

        /* Peripheral interrupt Deinit*/
        HAL_NVIC_DisableIRQ(ETH_IRQn);
    
#endif
#if defined(TARGET_STM32F746NG)
//Discovery STM32F746 Board
        __ETH_CLK_DISABLE();
        /**ETH GPIO Configuration
           PC1     ------> ETH_MDC
           PA1     ------> ETH_REF_CLK
           PA2     ------> ETH_MDIO
           PA7     ------> ETH_CRS_DV
           PC4     ------> ETH_RXD0
           PC5     ------> ETH_RXD1
           PB11     ------> ETH_TX_EN
           PB12     ------> ETH_TXD0
           PB13     ------> ETH_TXD1
         */
        HAL_GPIO_DeInit(GPIOC,  GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5);

        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_7);

        HAL_GPIO_DeInit(GPIOG, GPIO_PIN_11 | GPIO_PIN_13 | GPIO_PIN_14);

        /* Peripheral interrupt Deinit*/
        HAL_NVIC_DisableIRQ(ETH_IRQn); 
#endif
}
}
/**
 * Ethernet Rx Transfer completed callback
 *
 * @param  heth: ETH handle
 * @retval None
 */
void HAL_ETH_RxCpltCallback(ETH_HandleTypeDef *heth)
{
    sys_sem_signal(&rx_ready_sem);
}


/**
 * Ethernet IRQ Handler
 *
 * @param  None
 * @retval None
 */
void ETH_IRQHandler(void)
{
    HAL_ETH_IRQHandler(&heth);
}



/**
 * In this function, the hardware should be initialized.
 * Called from eth_arch_enetif_init().
 *
 * @param netif the already initialized lwip network interface structure
 *        for this ethernetif
 */
static void stm32f7_low_level_init(struct netif *netif)
{
 
    HAL_StatusTypeDef hal_eth_init_status;
    uint8_t* sernoaddr=(uint8_t*)0x1FF0F420;
    uint32_t serno =0;
    uint32_t i;
    uint8_t sernoid;
    // sum over the Unique device ID register 96bits
    for(i=0;i<12;i++)
    {
        serno=serno+sernoaddr[i];
    }
    macaddress[0]=2;
    macaddress[1]=0;
    macaddress[2]=0;
    sernoid=(serno>>16)&0xff;
    macaddress[3]=sernoid;
    sernoid=(serno>>8)&0xff;
    macaddress[4]=sernoid;
    sernoid=(serno)&0xff;
    macaddress[5]=sernoid;
    /* Init ETH */

     heth.Instance = ETH;
     heth.Init.MACAddr = &macaddress[0];
     heth.Init.AutoNegotiation = ETH_AUTONEGOTIATION_ENABLE;
     heth.Init.Speed = ETH_SPEED_100M;
     heth.Init.DuplexMode = ETH_MODE_FULLDUPLEX;
     heth.Init.MediaInterface = ETH_MEDIA_INTERFACE_RMII;
     heth.Init.RxMode = ETH_RXINTERRUPT_MODE;
     heth.Init.ChecksumMode = ETH_CHECKSUM_BY_HARDWARE;
     heth.Init.PhyAddress = LAN8742A_PHY_ADDRESS;
     hal_eth_init_status = HAL_ETH_Init(&heth);

    //Configure the receive filter
     heth.Instance->MACFFR = ETH_MACFFR_HPF | ETH_MACFFR_HM;
    //Disable flow control
     heth.Instance->MACFCR = 0;
    //Enable store and forward mode
     heth.Instance->DMAOMR = ETH_DMAOMR_RSF | ETH_DMAOMR_TSF;
    if (hal_eth_init_status == HAL_OK) {
       /* Set netif link flag */
       netif->flags |= NETIF_FLAG_LINK_UP;
   }

    SCB_DisableDCache();


    /* Initialize Tx Descriptors list: Chain Mode */
    HAL_ETH_DMATxDescListInit(&heth, DMATxDscrTab, &Tx_Buff[0][0], ETH_TXBUFNB);

    /* Initialize Rx Descriptors list: Chain Mode  */
    HAL_ETH_DMARxDescListInit(&heth, DMARxDscrTab, &Rx_Buff[0][0], ETH_RXBUFNB);

    /* set MAC hardware address length */
    netif->hwaddr_len = ETHARP_HWADDR_LEN;

    /* set MAC hardware address */
    netif->hwaddr[0] = heth.Init.MACAddr[0];
    netif->hwaddr[1] = heth.Init.MACAddr[1];
    netif->hwaddr[2] = heth.Init.MACAddr[2];
    netif->hwaddr[3] = heth.Init.MACAddr[3];
    netif->hwaddr[4] = heth.Init.MACAddr[4];
    netif->hwaddr[5] = heth.Init.MACAddr[5];

    /* maximum transfer unit */
    netif->mtu = 1500;

    /* device capabilities */
    /* don't set NETIF_FLAG_ETHARP if this device is not an ethernet one */
    netif->flags |= NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP;

    /* Enable MAC and DMA transmission and reception */
    HAL_ETH_Start(&heth);

}

/**
 * This function should do the actual transmission of the packet. The packet is
 * contained in the pbuf that is passed to the function. This pbuf
 * might be chained.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @param p the MAC packet to send (e.g. IP packet including MAC addresses and type)
 * @return ERR_OK if the packet could be sent
 *         an err_t value if the packet couldn't be sent
 *
 * @note Returning ERR_MEM here if a DMA queue of your MAC is full can lead to
 *       strange results. You might consider waiting for space in the DMA queue
 *       to become availale since the stack doesn't retry to send a packet
 *       dropped because of memory failure (except for the TCP timers).
 */

static err_t stm32f7_low_level_output(struct netif *netif, struct pbuf *p)
{
    err_t errval;
    struct pbuf *q;
    uint8_t *buffer = (uint8_t*)(heth.TxDesc->Buffer1Addr);
    __IO ETH_DMADescTypeDef *DmaTxDesc;
    uint32_t framelength = 0;
    uint32_t bufferoffset = 0;
    uint32_t byteslefttocopy = 0;
    uint32_t payloadoffset = 0;
    DmaTxDesc = heth.TxDesc;
    bufferoffset = 0;


    sys_mutex_lock(&tx_lock_mutex);

    /* copy frame from pbufs to driver buffers */
    for (q = p; q != NULL; q = q->next) {
        /* Is this buffer available? If not, goto error */
        if ((DmaTxDesc->Status & ETH_DMATXDESC_OWN) != (uint32_t)RESET) {
            errval = ERR_USE;
            goto error;
        }

        /* Get bytes in current lwIP buffer */
        byteslefttocopy = q->len;
        payloadoffset = 0;

        /* Check if the length of data to copy is bigger than Tx buffer size*/
        while ((byteslefttocopy + bufferoffset) > ETH_TX_BUF_SIZE) {
            /* Copy data to Tx buffer*/
            memcpy((uint8_t*)((uint8_t*)buffer + bufferoffset), (uint8_t*)((uint8_t*)q->payload + payloadoffset), (ETH_TX_BUF_SIZE - bufferoffset));

            /* Point to next descriptor */
            DmaTxDesc = (ETH_DMADescTypeDef*)(DmaTxDesc->Buffer2NextDescAddr);

            /* Check if the buffer is available */
            if ((DmaTxDesc->Status & ETH_DMATXDESC_OWN) != (uint32_t)RESET) {
                errval = ERR_USE;
                goto error;
            }

            buffer = (uint8_t*)(DmaTxDesc->Buffer1Addr);

            byteslefttocopy = byteslefttocopy - (ETH_TX_BUF_SIZE - bufferoffset);
            payloadoffset = payloadoffset + (ETH_TX_BUF_SIZE - bufferoffset);
            framelength = framelength + (ETH_TX_BUF_SIZE - bufferoffset);
            bufferoffset = 0;
        }

        /* Copy the remaining bytes */
        memcpy((uint8_t*)((uint8_t*)buffer + bufferoffset), (uint8_t*)((uint8_t*)q->payload + payloadoffset), byteslefttocopy);
        bufferoffset = bufferoffset + byteslefttocopy;
        framelength = framelength + byteslefttocopy;
    }

    /* Prepare transmit descriptors to give to DMA */
    HAL_ETH_TransmitFrame(&heth, framelength);
		LWIP_DEBUGF(NETIF_DEBUG,("(SYS_Tick=0x%x)STM_low_level_output: %d bytes \r\n",HAL_GetTick() ,framelength));
    errval = ERR_OK;

error:

    /* When Transmit Underflow flag is set, clear it and issue a Transmit Poll Demand to resume transmission */
    if ((heth.Instance->DMASR & ETH_DMASR_TUS) != (uint32_t)RESET) {
        /* Clear TUS ETHERNET DMA flag */
        heth.Instance->DMASR = ETH_DMASR_TUS;

        /* Resume DMA transmission*/
        heth.Instance->DMATPDR = 0;
    }

    sys_mutex_unlock(&tx_lock_mutex);
    #ifdef EMAC_TRANSMIT_WAIT
    wait_us(EMAC_T_WAIT);
    #endif
    return errval;
}


/**
 * Should allocate a pbuf and transfer the bytes of the incoming
 * packet from the interface into the pbuf.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return a pbuf filled with the received packet (including MAC header)
 *         NULL on memory error
 */
static struct pbuf * stm32f7_low_level_input(struct netif *netif)
{
    struct pbuf *p = NULL;
    struct pbuf *q = NULL;
    uint16_t len = 0;
    uint8_t *buffer;
    __IO ETH_DMADescTypeDef *dmarxdesc;
    uint32_t bufferoffset = 0;
    uint32_t payloadoffset = 0;
    uint32_t byteslefttocopy = 0;
    uint32_t i = 0;

    /* get received frame */
    if (HAL_ETH_GetReceivedFrame_IT(&heth) != HAL_OK)
        return NULL;

    /* Obtain the size of the packet and put it into the "len" variable. */
    len = heth.RxFrameInfos.length;
    buffer = (uint8_t*)heth.RxFrameInfos.buffer;

    if (len > 0) {
        /* We allocate a pbuf chain of pbufs from the Lwip buffer pool */
        p = pbuf_alloc(PBUF_RAW, len, PBUF_RAM);

    }

    if (p != NULL) {
        dmarxdesc = heth.RxFrameInfos.FSRxDesc;
        bufferoffset = 0;
        for (q = p; q != NULL; q = q->next) {
            byteslefttocopy = q->len;
            payloadoffset = 0;

            /* Check if the length of bytes to copy in current pbuf is bigger than Rx buffer size*/
            while ((byteslefttocopy + bufferoffset) > ETH_RX_BUF_SIZE) {
                /* Copy data to pbuf */
                memcpy((uint8_t*)((uint8_t*)q->payload + payloadoffset), (uint8_t*)((uint8_t*)buffer + bufferoffset), (ETH_RX_BUF_SIZE - bufferoffset));
                /* Point to next descriptor */
                dmarxdesc = (ETH_DMADescTypeDef*)(dmarxdesc->Buffer2NextDescAddr);
                buffer = (uint8_t*)(dmarxdesc->Buffer1Addr);

                byteslefttocopy = byteslefttocopy - (ETH_RX_BUF_SIZE - bufferoffset);
                payloadoffset = payloadoffset + (ETH_RX_BUF_SIZE - bufferoffset);
                bufferoffset = 0;
            }
            /* Copy remaining data in pbuf */
            memcpy((uint8_t*)((uint8_t*)q->payload + payloadoffset), (uint8_t*)((uint8_t*)buffer + bufferoffset), byteslefttocopy);
            bufferoffset = bufferoffset + byteslefttocopy;
        }
        /* Release descriptors to DMA */
        /* Point to first descriptor */
        dmarxdesc = heth.RxFrameInfos.FSRxDesc;
        /* Set Own bit in Rx descriptors: gives the buffers back to DMA */
        for (i = 0; i < heth.RxFrameInfos.SegCount; i++) {
            dmarxdesc->Status |= ETH_DMARXDESC_OWN;
            dmarxdesc = (ETH_DMADescTypeDef*)(dmarxdesc->Buffer2NextDescAddr);
        }

        /* Clear Segment_Count */
        heth.RxFrameInfos.SegCount = 0;
        LWIP_DEBUGF(NETIF_DEBUG,("(SYS_Tick=0x%x)STM_low_level_input: Packet received: %p, len %d \r\n",HAL_GetTick(),p, p->len));
    }
    else
    {
        LWIP_DEBUGF(NETIF_DEBUG,("STM_low_level_input: Packet dropped \r\n"));
    }

    /* When Rx Buffer unavailable flag is set: clear it and resume reception */
    if ((heth.Instance->DMASR & ETH_DMASR_RBUS) != (uint32_t)RESET) {
        /* Clear RBUS ETHERNET DMA flag */
        heth.Instance->DMASR = ETH_DMASR_RBUS;
        /* Resume DMA reception */
        heth.Instance->DMARPDR = 0;
    }
    #ifdef EMAC_RECIVE_WAIT
    wait_us(EMAC_R_WAIT);
    #endif
    return p;
}

/**
 * This task receives input data
 *
 * \param[in] netif the lwip network interface structure
 */
static void stm32f7_rx_task(void *arg)
{
    struct netif   *netif = (struct netif*)arg;
    struct pbuf    *p;
    struct eth_hdr *ethhdr;
    while (1) {
        sys_arch_sem_wait(&rx_ready_sem, 0);
        #ifdef LOCK_RX_THREAD
        sys_mutex_lock(&tx_lock_mutex);
        #endif
        sysflag=0x0000;
        p = stm32f7_low_level_input(netif);
        if (p != NULL) {
            ethhdr = p->payload;
            switch (htons(ethhdr->type))
            {
                /* IP or ARP packet? */
               case ETHTYPE_IP:
               case ETHTYPE_ARP:
                   if (netif->input(p, netif) != ERR_OK) {
                    LWIP_DEBUGF(NETIF_DEBUG, ("STM_enetif_input: IP input error\n"));
                    pbuf_free(p);
                    p = NULL;
                    }
                    break;
               default:
                    LWIP_DEBUGF(NETIF_DEBUG, ("STM_enetif_input: Payload not IP or ARP %d \r\n", p->payload));
                    pbuf_free(p);
                    p = NULL;
                    break;
            }

        }
      #ifdef LOCK_RX_THREAD
      sys_mutex_unlock(&tx_lock_mutex);
      #endif
    }
}

/**
 * This task checks phy link status and updates net status
 *
 * \param[in] netif the lwip network interface structure
 */
static void stm32f7_phy_task(void *arg)
{
    struct netif   *netif = (struct netif*)arg;


    while (1) {
        uint32_t status;
        if (HAL_ETH_ReadPHYRegister(&heth, PHY_BSR, &status) == HAL_OK)
        {
          if (phy_status != status)
           {
            if (status & PHY_LINKED_STATUS )
             {
              tcpip_callback_with_block((tcpip_callback_fn)netif_set_link_up, (void*) netif, 1);
             }
            else
             {
              tcpip_callback_with_block((tcpip_callback_fn)netif_set_link_down, (void*) netif, 1);
             }
            }
        }
        phy_status = status;
    }

    osDelay(PHY_TASK_WAIT);

}

/**
 * This function is the ethernet packet send function. It calls
 * etharp_output after checking link status.
 *
 * \param[in] netif the lwip network interface structure for this lpc_enetif
 * \param[in] q Pointer to pbug to send
 * \param[in] ipaddr IP address
 * \return ERR_OK or error code
 */
static err_t stm32f7_etharp_output(struct netif *netif, struct pbuf *q, ip_addr_t *ipaddr)
{
    /* Only send packet is link is up */
    if (netif->flags & NETIF_FLAG_LINK_UP) {
        return etharp_output(netif, q, ipaddr);
    }

    return ERR_CONN;
}

/**
 * Should be called at the beginning of the program to set up the
 * network interface.
 *
 * This function should be passed as a parameter to netif_add().
 *
 * @param[in] netif the lwip network interface structure for this lpc_enetif
 * @return ERR_OK if the loopif is initialized
 *         ERR_MEM if private data couldn't be allocated
 *         any other err_t on error
 */
err_t eth_arch_enetif_init(struct netif *netif)
{
    uint8_t* sernoaddr=0x1FF0F420;
    uint32_t serno =0;
    uint32_t i;
    uint8_t sernoid;
    // sum over the Unique device ID register 96bits
    for(i=0;i<12;i++)
    {
        serno=serno+sernoaddr[i];
    }
    macaddress[0]=2;
    macaddress[1]=0;
    macaddress[2]=0;
    sernoid=(serno>>16)&0xff;
    macaddress[3]=sernoid;
    sernoid=(serno>>8)&0xff;
    macaddress[4]=sernoid;
    sernoid=(serno)&0xff;
    macaddress[5]=sernoid;

    /* set MAC hardware address */
    netif->hwaddr_len = ETHARP_HWADDR_LEN;


  /* set netif MAC hardware address */
    netif->hwaddr[0] = macaddress[0];
    netif->hwaddr[1] = macaddress[1];
    netif->hwaddr[2] = macaddress[2];
    netif->hwaddr[3] = macaddress[3];
    netif->hwaddr[4] = macaddress[4];
    netif->hwaddr[5] = macaddress[5];

    /* maximum transfer unit */
    netif->mtu = 1500;

    /* device capabilities */
    //netif->flags |= NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP;

    netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_ETHERNET | NETIF_FLAG_IGMP;

#if LWIP_NETIF_HOSTNAME
    /* Initialize interface hostname */
    netif->hostname = NETIF_HOSTNAME;
#endif /* LWIP_NETIF_HOSTNAME */

    netif->name[0] = 'e';
    netif->name[1] = 'n';

    netif->output = stm32f7_etharp_output;
    netif->linkoutput = stm32f7_low_level_output;

    /* semaphore */
    sys_sem_new(&rx_ready_sem, 0);

    sys_mutex_new(&tx_lock_mutex);

    /* task */
    sys_thread_new("recv_task", stm32f7_rx_task, netif, EMAC_RECIVE_THREAD_STACKSIZE, RECV_TASK_PRI);
    sys_thread_new("phy_task", stm32f7_phy_task, netif, DEFAULT_THREAD_STACKSIZE, PHY_TASK_PRI);

    /* initialize the hardware */
    stm32f7_low_level_init(netif);
    netif_set_link_down(netif);
    HAL_Delay(100);
    phy_status=0;
    return ERR_OK;
}

void eth_arch_enable_interrupts(void)
{
  /* Enable the Ethernet global Interrupt */
  HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
  HAL_NVIC_SetPriority(ETH_IRQn, 0x7, 0);
  HAL_NVIC_EnableIRQ(ETH_IRQn);
}

void eth_arch_disable_interrupts(void)
{
    NVIC_DisableIRQ(ETH_IRQn);
}

/**
 * @}
 */

/* --------------------------------- End Of File ------------------------------ */
