/**
 * \file
 * CAN_Transmit.cc
 * Description:
 * This file contains Functions which Select which image to send
 * also Message is composed in CAN frame format and send from receiver and
 * Message is printed on Terminal using UART.
 *
 *  Author:
 *  Nandita Ekbote
 *  Himanshu Devi
 *  Created on: 15-Mar-2018
 */

#include "main.hh"

/**
 * \fn
 * Description:
 * This function is used to read the CANINTE register for Transmit buffer 0
 * Switch statement selects the Image that want to transmit
 * @param rgbRead is a integer pointer which stores the base address of the data read from the socket
 */
int ID = 000;
int Array_size = 0;
uint8_t *Array_ptr = NULL;
int array_index = 0;

void select(int *rgbRead)
{
  int i,Bytes_Transfer;
  do {
      //!* Reads the status of the CAN device from status Register */
      status = CAN_ReadStatus(&myDevice);
  } while ((status & CAN_STATUS_TX0REQ_MASK) != 0); //!* wait for buffer 0 to be clear */


  switch((*rgbRead) - ASCII_CONSTANT)
  {
    case 1:
      Array_ptr = Radioarray;
      Array_size = (sizeof(Radioarray))/sizeof(int);
      break;

    case 2:
      Array_ptr = PlayMusicarray;
      Array_size = (sizeof(PlayMusicarray))/sizeof(int);
      break;

    case 3:
      Array_ptr = galleryiconarray;
      Array_size = (sizeof(galleryiconarray))/sizeof(int);
      break;

    case 4:
      Array_ptr = MoviePlayarray;
      Array_size = (sizeof(MoviePlayarray))/sizeof(int);
      break;

    default: Array_ptr = NULL;
  }

  //!*The loop will execute for Array_size/8 times which will compose no of bytes in CAN frame */
  for(Bytes_Transfer = 0 ; Bytes_Transfer < (Array_size/8); Bytes_Transfer++)
    {
      //!*Loop to compose 8 bytes in a one CAN frame */
      for (i=0; i<8; i++)
	{

	  if(array_index != (Array_size))
	    {
	      data_bytes[i] = Array_ptr[array_index];
	      array_index++;
	    }
      //!* If all frames are transferred then send one last frame as all FF's so that Image can be displayed */
	  else
	    {
	      data_bytes[i] = 0xFF;
	    }
	}

      send(data_bytes);

      do {
	  status = CAN_ReadStatus(&myDevice);
	  xil_printf("Waiting to complete transmission\r\n");
      } while ((status & CAN_STATUS_TX0IF_MASK) != 0); /*! wait for message to transmit successfully*/

    }

  //!* If wrong option is selected then send all FF's
  if((*rgbRead - ASCII_CONSTANT))
    {
      data_bytes[0] = 0xFF;
      data_bytes[1] = 0xFF;
      data_bytes[2] = 0xFF;
      data_bytes[3] = 0xFF;
      data_bytes[4] = 0xFF;
      data_bytes[5] = 0xFF;
      data_bytes[6] = 0xFF;
      data_bytes[7] = 0xFF;

      send(data_bytes);

      /*! Reads the status register. If Tx0IF bit is set then the transmission is
       * not performed until bit is 0*/
      do {
	  status = CAN_ReadStatus(&myDevice);
	  xil_printf("Waiting to complete transmission\r\n");
      } while ((status & CAN_STATUS_TX0IF_MASK) != 0);

      ID = 0;
    }
  array_index = 0;

}

/**
 * \fn
 * Description:
 * This Function stores the composed frame into TxMessage Structure variable and prints the current Frame on terminal.
 * It modifies Command register and reads CAN INTERRUPT FLAG REGISTER.
 * It sends TxMessage by calling CAN_SendMessage to transmit buffer of the current CAN device.
 * @param data_bytes is a integer pointer that stores the base address of the 8 bytes which are to be send.
 */
void send(int* data_bytes)
{
  TxMessage = ComposeMessage(data_bytes);

  xil_printf("sending...");
  PrintMessage(TxMessage);

  CAN_ModifyReg(&myDevice, CAN_CANINTF_REG_ADDR, CAN_CANINTF_TX0IF_MASK, 0);

  xil_printf("requesting to transmit message through transmit buffer 0\r\n");

  CAN_SendMessage(&myDevice, TxMessage, CAN_Tx0);

  CAN_ModifyReg(&myDevice, CAN_CANINTF_REG_ADDR, CAN_CANINTF_TX0IF_MASK, 0);
}

/**
 * \fn
 * Description:
 * This function composes the CAN message that is to be transmitted.
 * @param frame_bytes is a integer pointer which stores the base address of each frame of 8 bytes having different ID.
 * @return CAN_Message structure.
 */
CAN_Message ComposeMessage(int* frame_bytes)
{
  CAN_Message message;
  xil_printf("Composing.....");
  message.id = ID;
  message.dlc = 8;
  message.eid = 0x15a;
  message.rtr = 0;
  message.ide = 0;
  for(int i=0; i< message.dlc ; i++)
    {
      message.data[i] = frame_bytes[i];
      xil_printf("MSG = %d\n",message.data[i]);
    }
  ID++;
  return message;
}

/**
 * \fn
 * Description:
 * This function Prints the CAN Frame or CAN message
 * @param message is a structure variable of CAN_Message structure
 */
void PrintMessage(CAN_Message message)
{
  u8 i;

  xil_printf("message:\r\n");

  xil_printf("    %s Frame\r\n", (message.ide) ? "Extended" : "Standard");
  xil_printf("    ID: %03x\r\n", message.id);

  if (message.ide)
    xil_printf("    EID: %05x\r\n", message.eid);

  if (message.rtr)
    xil_printf("    Remote Transmit Request\r\n");

  else
    xil_printf("    Standard Data Frame\r\n");

  xil_printf("    dlc: %01x\r\n", message.dlc);
  xil_printf("    data:\r\n");

  for (i=0; i<message.dlc; i++)
    xil_printf("        %02x\r\n", message.data[i]);
}


