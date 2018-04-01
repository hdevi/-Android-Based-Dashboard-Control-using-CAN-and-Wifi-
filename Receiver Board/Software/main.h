/*!
 *	\file
 *	main.h
 *	Description:
 *	This file contains Macros, Function declaration, variable declaration\
 *	Author:
	Nandita Ekbote
	Himanshu Devi

	Created: 15 - March - 2018
 *	*/

#ifndef SRC_MAIN_H_
#define SRC_MAIN_H_

#include "xparameters.h"
#include "xil_cache.h"
#include "PmodCAN.h"
#include "sleep.h"
#include "PmodOLEDrgb.h"
#include <stdlib.h>

//!*\def Definitions for Pmod CAN */
#define PMODCAN_DEVICE_ID		XPAR_PMODCAN_0_DEVICE_ID
#define PMODCAN_GPIO_BASEADDR	XPAR_PMODCAN_0_AXI_LITE_GPIO_BASEADDR
#define PMODCAN_HIGHADDR		XPAR_NEXYS4IO_0_S00_AXI_HIGHADDR
#define PMODCAN_SPI_BASEADDR	XPAR_PMODCAN_0_AXI_LITE_SPI_BASEADDR
#define PMODCAN_HIGHADDR		XPAR_NEXYS4IO_0_S00_AXI_HIGHADDR

//!*\def Definitions for peripheral PMODOLEDRGB */
#define PMODOLEDRGB_DEVICE_ID		XPAR_PMODOLEDRGB_0_DEVICE_ID
#define PMODOLEDRGB_GPIO_BASEADDR	XPAR_PMODOLEDRGB_0_AXI_LITE_GPIO_BASEADDR
#define PMODOLEDRGB_GPIO_HIGHADDR	XPAR_PMODOLEDRGB_0_AXI_LITE_GPIO_HIGHADD
#define PMODOLEDRGB_SPI_BASEADDR	XPAR_PMODOLEDRGB_0_AXI_LITE_SPI_BASEADDR
#define PMODOLEDRGB_SPI_HIGHADDR	XPAR_PMODOLEDRGB_0_AXI_LITE_SPI_HIGHADDR

//!* Function Declarations */
int init(void);
void ReceiverRun();
void Cleanup();
void PrintMessage(CAN_Message message);
void EnableCaches();
void DisableCaches();
void PMDIO_itoa(int32_t value, char *string, int32_t radix);
void PMDIO_puthex(PmodOLEDrgb* InstancePtr, uint32_t num);
void PMDIO_putnum(PmodOLEDrgb* InstancePtr, int32_t num, int32_t radix);

//!*\var Rx_node PmodCAN instance */
PmodCAN Rx_node;

//!*\var pmodOLEDrgb_inst PmodOLEDrgb instance */
PmodOLEDrgb	pmodOLEDrgb_inst;

//!*\var input_array is a array used to store data received from transmitter */
extern uint8_t input_array[12288];

//!*\var elements is used to calculate size of array
int elements;

//!*\var input_array_ptr is a pointer which stores the base address of input_array*/
extern uint8_t *input_array_ptr ;

#endif /* SRC_MAIN_H_ */
