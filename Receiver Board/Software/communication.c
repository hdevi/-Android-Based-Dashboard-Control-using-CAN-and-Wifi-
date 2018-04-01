/*!
 *	\file
 *	communication.c
 *	Description:
 *	This file has functions which are invoked during CAN message reception
	Author:
	Nandita Ekbote
	Himanshu Devi

	Created: 15 - March - 2018
*/

#include "main.h"

uint8_t input_array[12288] = {0};
uint8_t *input_array_ptr = input_array;


//!*
//! \fn
//! Description:
//! This function is used to print message format of each frame
//! @param message is a variable of type CAN_Message structure*/
void PrintMessage(CAN_Message message)
{
	u8 i;
	xil_printf("message:\r\n");
	xil_printf("        %02x\r\n", message.id);
	xil_printf("    dlc: %01x\r\n", message.dlc);
	xil_printf("    data:\r\n");

	for (i=0; i<message.dlc; i++)
	{
		xil_printf("        %02x\r\n", message.data[i]);
	}
}

/*!
 * \fn
 * Description:
 * This function checks the status of Pmod CAN instance
 * Checks which Rx buffer has data in it. It waits until there is data in anyone of the buffer
 * selects the buffer which has data and gets message from it and copies it into RxMsg
 * clears the interrupt bit in CANINTF Register
 * */
void ReceiverRun()
{
	//!*\var RxMsg is variable which is used to store received message in CAN_Message format*/
	CAN_Message RxMsg;

	//!*\var RxBufferNo is variable used to select which buffer the data is in*/
	CAN_RxBuffer RxBufferNo;
	u8 status;
	//!*Rx_mask is used to store mask for Receive Buffer*/
	u8 Rx_mask;
	//!*array_index is variable used to store the current array index*/
	int array_index = 0;

	while (1)
	{

		//!* check if message is received in Rx buffer */
		do {
			status = CAN_ReadStatus(&Rx_node);
			xil_printf("Waiting to receive\r\n");
		} while ((status & CAN_STATUS_RX0IF_MASK) != 0 && (status & CAN_STATUS_RX1IF_MASK) != 0); //!*Checks which buffer*/

		//!* Find which Rx buffer has data*/
		switch(status & 0x03)
		{
		case 0b01:
		case 0b11:
			xil_printf("fetching message from receive buffer 0\r\n");
			RxBufferNo = CAN_Rx0;
			Rx_mask = CAN_CANINTF_RX0IF_MASK;
			break;
		case 0b10:
			xil_printf("fetching message from receive buffer 1\r\n");
			RxBufferNo = CAN_Rx1;
			Rx_mask = CAN_CANINTF_RX1IF_MASK;
			break;
		default:
			xil_printf("Error, message not received\r\n");
			continue;
		}

		//!* Get message from Rx buffer */
		CAN_ReceiveMessage(&Rx_node, &RxMsg, RxBufferNo);

		//!* Clear the interrupt flag */
		CAN_ModifyReg(&Rx_node, CAN_CANINTF_REG_ADDR, Rx_mask, 0);

		xil_printf("received ");

		//!*If message ID is 0 then byte 1 of that message is used to select what image was pressed
		//! and what text should be displayed on OLEDrgb*/
		if(RxMsg.id == 0)
		{
			xil_printf("Id = %x ",RxMsg.id );
			OLEDrgb_SetFontColor(&pmodOLEDrgb_inst,OLEDrgb_BuildRGB(255, 255, 255));
			switch(RxMsg.data[0])
			{
			case 1:
				xil_printf("Radio ");
				OLEDrgb_Clear(&pmodOLEDrgb_inst);
				OLEDrgb_SetCursor(&pmodOLEDrgb_inst, 0, 3);
				OLEDrgb_PutString(&pmodOLEDrgb_inst,"Radio ");
				OLEDrgb_SetCursor(&pmodOLEDrgb_inst, 0, 5);
				OLEDrgb_PutString(&pmodOLEDrgb_inst,"connecting..");
				break;

			case 2:
				OLEDrgb_Clear(&pmodOLEDrgb_inst);
				OLEDrgb_SetCursor(&pmodOLEDrgb_inst, 0, 3);
				OLEDrgb_PutString(&pmodOLEDrgb_inst,"Playing your");
				OLEDrgb_SetCursor(&pmodOLEDrgb_inst, 0, 5);
				OLEDrgb_PutString(&pmodOLEDrgb_inst,"fav song..");
				break;

			case 3:
				OLEDrgb_Clear(&pmodOLEDrgb_inst);
				OLEDrgb_SetCursor(&pmodOLEDrgb_inst, 0, 3);
				OLEDrgb_PutString(&pmodOLEDrgb_inst,"Your Snaps..");
				OLEDrgb_SetCursor(&pmodOLEDrgb_inst, 0, 5);
				OLEDrgb_PutString(&pmodOLEDrgb_inst,"Just COOL!");
				break;

			case 4:
				OLEDrgb_Clear(&pmodOLEDrgb_inst);
				OLEDrgb_SetCursor(&pmodOLEDrgb_inst, 0, 1);
				OLEDrgb_PutString(&pmodOLEDrgb_inst," Loading");
				OLEDrgb_SetCursor(&pmodOLEDrgb_inst, 0, 3);
				OLEDrgb_PutString(&pmodOLEDrgb_inst," movie.. ");
				OLEDrgb_SetCursor(&pmodOLEDrgb_inst, 0, 5);
				OLEDrgb_PutString(&pmodOLEDrgb_inst," Grab ");
				OLEDrgb_SetCursor(&pmodOLEDrgb_inst, 0, 7);
				OLEDrgb_PutString(&pmodOLEDrgb_inst," popcorn!");
				break;
			}
		}

		//!* check if all data bytes are 0xFF, if yes then end of image */
		if((RxMsg.data[0] == 0xFF) &&  (RxMsg.data[1] == 0xFF) &&  (RxMsg.data[2] == 0xFF) &&  (RxMsg.data[3] == 0xFF) &&  (RxMsg.data[4] == 0xFF) &&  (RxMsg.data[5] == 0xFF) &&  (RxMsg.data[6] == 0xFF) &&  (RxMsg.data[7] == 0xFF))
		{
			for (int i=0; i<8; i++)
			{
				xil_printf("        %02x\r\n", RxMsg.data[i]);

			}
			OLEDrgb_Clear(&pmodOLEDrgb_inst);
			//!*Bitmap image is displayed on OLEDrgb passing pointer to it*/
			OLEDrgb_DrawBitmap(&pmodOLEDrgb_inst,0,0,95,63,(uint8_t*) input_array_ptr);

			array_index = 0;
			//!*Size of array calculated and stored in elements */
			elements = (sizeof(input_array)/sizeof(char));

			//!* Again array is initialized to 0*/
			for(int i =0; i< elements; i++)
				input_array[i] = 0;
			input_array_ptr = NULL;
		}
		//!* Write data into array only if */
		else
		{
			for (int i=0; i<RxMsg.dlc; i++)
			{
				input_array[array_index] = RxMsg.data[i];
				array_index++;
			}
			PrintMessage(RxMsg);
		}
	}
}

/*!
 * \fn
 * Description:
 * This function is used initialize the Pmod CAN and Pmod OLEDrgb
 * It configures CAN in Normal mode
 * @return It returns if the initialization is Success or failed
 * */
int init(void)
{
	EnableCaches();
	CAN_begin(&Rx_node, PMODCAN_GPIO_BASEADDR, PMODCAN_SPI_BASEADDR);
	CAN_Configure(&Rx_node, CAN_ModeNormalOperation);
	OLEDrgb_begin(&pmodOLEDrgb_inst, PMODOLEDRGB_GPIO_BASEADDR, PMODOLEDRGB_SPI_BASEADDR);
	OLEDrgb_Clear(&pmodOLEDrgb_inst);
	OLEDrgb_SetFontColor(&pmodOLEDrgb_inst,OLEDrgb_BuildRGB(200, 12, 44));
	OLEDrgb_SetCursor(&pmodOLEDrgb_inst, 2, 3);
	OLEDrgb_PutString(&pmodOLEDrgb_inst,"Hi There! ");
	sleep(1);

	return XST_SUCCESS;
}
