/**
 * \file
 *  communication.cc
 *  Description:
 *  This file contains Communication function which is used to communicate with Android device
 *  through Sockets
 *
 *  Author:
 *  Nandita Ekbote
 *  Himanshu Devi
 *  Created on: 15-Mar-2018
 *
 */

#include "main.hh"

/**
 * Description:
 * This Function implements a State transition diagram for Server to get connected and
 * communicate with client.
 * It calls function from diligent Embedded IP stack which is based on TCP/IP stack which is implemented
 * in C++
 */

IPv4 ipServer = {192,168,43,190};
unsigned short portServer = DEIPcK::iPersonalPorts44 + 300;
unsigned tWait = 5000;
TCPSocket *ptcpClient = NULL;
int cbRead = 0;
int count = 0;
unsigned tStart = 0;
//!*\var state is initialized to CONNECT
STATE state = CONNECT;
//!*\var szPassPhrase is String constant*/
const char *szPassPhrase = "12344321";
//!*\var szSssid is string constant to Specify the SSID */
const char *szSsid = "PSU";

void communication()
{
  switch (state) {
    case CONNECT:
      if (WiFiConnectMacro())
	{
	  xil_printf("Connection Created\r\n");
	  deIPcK.begin(ipServer);
	  state = LISTEN;
	} else if (IsIPStatusAnError(status)) {
	    xil_printf("Unable to make connection, status: 0x%X\r\n", status);
	    state = CLOSE;
	}
      break;

      //!* Say to listen on the port*/
    case LISTEN:
      if (deIPcK.tcpStartListening(portServer, tcpServer)) {
	  for (int i = 0; i < cTcpClients; i++) {
	      tcpServer.addSocket(rgTcpClient[i]);
	  }
      }
      state = ISLISTENING;
      break;

    case ISLISTENING:
      count = tcpServer.isListening();

      if (count > 0) {
	  deIPcK.getMyIP(ipServer);
	  xil_printf("Server started on %d.%d.%d.%d:%d\r\n",
		     ipServer.u8[0],
		     ipServer.u8[1],
		     ipServer.u8[2],
		     ipServer.u8[3],
		     portServer
	  );
	  xil_printf("%d sockets listening on port: %d\r\n", count,
		     portServer);
	  state = AVAILABLECLIENT;
      } else {
	  xil_printf("%d sockets listening on port: %d\r\n", count,
		     portServer);
	  state = WAITISLISTENING;
      }
      break;

    case WAITISLISTENING:
      if (tcpServer.isListening() > 0) {
	  state = ISLISTENING;
      }
      break;

      //!* Wait for a connection*/
    case AVAILABLECLIENT:
      if ((count = tcpServer.availableClients()) > 0) {
	  xil_printf("Got %d clients pending\r\n", count);
	  state = ACCEPTCLIENT;
      }
      break;

      //!* Accept the connection*/
    case ACCEPTCLIENT:

      //!* Accept the client*/
      if ((ptcpClient = tcpServer.acceptClient()) != NULL
	  && ptcpClient->isConnected()) {
	  xil_printf("Got a Connection\r\n");
	  state = READ;
	  tStart = (unsigned) SYSGetMilliSecond();
      }

      /*! This probably won't happen unless the connection is dropped
  	   if it is, just release our socket and go back to listening */
      else {
	  state = CLOSE;
      }
      break;

      /*! Wait for the read, but if too much time elapses (5 seconds)
  	   we will just close the tcpClient and go back to listening */
    case READ:

      //!* See if we got anything to read */
      if ((cbRead = ptcpClient->available()) > 0) {
	  cbRead = cbRead < (int) sizeof(rgbRead) ? cbRead : sizeof(rgbRead);
	  cbRead = ptcpClient->readStream(rgbRead, cbRead);
	  xil_printf("Got %d bytes\r\n", cbRead);
	  state = WRITE;
      }

      //!* If connection was closed by the user */
      else if (!ptcpClient->isConnected()) {
	  state = CLOSE;
      }
      break;

      //!* Echo back the string */
    case WRITE:
      if (ptcpClient->isConnected()) {
	  xil_printf("Writing: \r\n");
	  for (int i = 0; i < cbRead; i++) {
	      xil_printf("%d", (int) rgbRead[i]);

	      select((int*)rgbRead);
	  }
	  xil_printf("\r\n");

	  ptcpClient->writeStream(rgbRead, cbRead);
	  state = READ;


	  tStart = (unsigned) SYSGetMilliSecond();
      }

      //!* The connection was closed on us, go back to listening */
      else {
	  xil_printf("Unable to write back.\r\n");
	  state = CLOSE;
      }
      break;

      //!* Close our tcpClient and go back to listening */
    case CLOSE:
      xil_printf("Closing connection\r\n");
      if (ptcpClient)
	ptcpClient->close();
      tcpServer.addSocket(*ptcpClient);
      xil_printf("\r\n");
      state = ISLISTENING;
      break;

      //!* Something bad happened, just exit out of the program */
    case EXIT:
      tcpServer.close();
      xil_printf("Something went wrong, sketch is done.\r\n");
      state = DONE;
      break;

      //!* Do nothing in the loop */
    case DONE:

    default:
      break;
  }
}


