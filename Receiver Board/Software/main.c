/*!
 *	\file
 *	main.c
 *	Description:
 *	This file contains Entry point function i.e main
 *	It initializes the peripherals and Runs the CAN receiver
 *	The file contains Enable and Disable Cache function
	Author:
	Nandita Ekbote
	Himanshu Devi

	Created: 15 - March - 2018
*/

#include "main.h"

int main(void)
{
	int status;
	//!*initializing the peripherals */
	status = init();
	if (status != XST_SUCCESS )
	{
		xil_printf("failed to init");
		exit(1);
	}

	ReceiverRun();

	Cleanup();
	return 0;
}

/**
 * \fn
 * This function Ends CAN transmit and disables Caches
 */
void Cleanup()
{
	CAN_end(&Rx_node);
	DisableCaches();
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


