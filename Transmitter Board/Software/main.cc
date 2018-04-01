/**
 * \file
 *  main.cc
 *  This file contains Entry point Function i.e main function which is continuously running
 *  for Server to communicate with client
 *
 *  Author:
 *  Nandita Ekbote
 *  Himanshu Devi
 *  Created on: 15-Mar-2018
 *
 */

#include "main.hh"

int main()
{
  //!*Initialize the Pmods */
  Initialize();

  xil_printf("PMOD WIFI and CAN are configured\n ");

  while(1)
    {
      communication();
      // Every pass through loop(), keep the stack alive
      DEIPcK::periodicTasks();
    }

  cleanup();
  return 0;
}


/**
 * \fn
 * Description:
 * This function Initializes Wifi and CAN.
 * Configures CAN in Normal Mode configuration
 * Enable Caches
 */
void Initialize()
{
  EnableCaches();
  Wifi_Initialize();
  CAN_begin(&myDevice, XPAR_PMODCAN_0_AXI_LITE_GPIO_BASEADDR, XPAR_PMODCAN_0_AXI_LITE_SPI_BASEADDR);
  CAN_Configure(&myDevice,  CAN_ModeNormalOperation);
}

/**
 * \fn
 * Description:
 * This Initializes Wifi Pmod
 */
void Wifi_Initialize()
{
  setPmodWifiAddresses(
      XPAR_PMODWIFI_0_AXI_LITE_SPI_BASEADDR,
      XPAR_PMODWIFI_0_AXI_LITE_WFGPIO_BASEADDR,
      XPAR_PMODWIFI_0_AXI_LITE_WFCS_BASEADDR,
      XPAR_PMODWIFI_0_S_AXI_TIMER_BASEADDR
  );
  setPmodWifiIntVector(PMODWIFI_VEC_ID);
}

/**
 * \fn
 * Description:
 * Enables ICACHE and DCACHE for microblaze
 */
void EnableCaches()
{
#ifdef __MICROBLAZE__
#ifdef XPAR_MICROBLAZE_USE_ICACHE
  Xil_ICacheEnable();
#endif
#ifdef XPAR_MICROBLAZE_USE_DCACHE
  Xil_DCacheEnable();
#endif
#endif
}

/**
 * \fn
 * Description:
 * Disables ICACHE and DCACHE for microblaze
 */
void DisableCaches()
{
#ifdef __MICROBLAZE__
#ifdef XPAR_MICROBLAZE_USE_DCACHE
  Xil_DCacheDisable();
#endif
#ifdef XPAR_MICROBLAZE_USE_ICACHE
  Xil_ICacheDisable();
#endif
#endif
}

/**
 * \fn
 * This function Ends CAN transmit and disables Caches
 */
void cleanup()
{
  CAN_end(&myDevice);
  DisableCaches();
}

