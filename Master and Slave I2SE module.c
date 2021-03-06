/** ###################################################################
**     Filename    : ProcessorExpert.c
**     Project     : ProcessorExpert
**     Processor   : MK20DX256VLL7
**     Version     : Driver 01.01
**     Compiler    : GNU C Compiler
**     Date/Time   : 2013-03-19, 07:47, # CodeGen: 0
**     Abstract    :
**         Main module.
**         This module contains user's application code.
**     Settings    :
**     Contents    :
**         No public methods
**
** ###################################################################*/
/* MODULE ProcessorExpert */


/* Including needed modules to compile this module/procedure */
#include "Cpu.h"
#include "Events.h"
#include "LED1.h"
#include "LED2.h"
#include "LED3.h"
#include "LED4.h"
#include "SM1.h"
#include "SPI_INTR.h"
#include "TIMER.h"
#include "DIP1_4.h"
#include "DIP1_3.h"
#include "DIP1_2.h"
#include "DIP1_1.h"
#include "DIP2_4.h"
#include "CON_X3_8.h"
#include "CON_X3_7.h"
#include "CON_X3_6.h"
#include "CON_X3_5.h"
#include "CON_X3_4.h"
#include "CON_X3_3.h"
#include "QCA7K_RESET.h"
#include "AS1.h"
#include "RxBuf.h"
#include "UTIL1.h"
#include "AS2.h"
#include "ASerialLdd1.h"
#include "DIP2_3.h"
#include "DIP2_2.h"
#include "DIP2_1.h"
/* Including shared modules, which are used for whole project */
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"

/* User includes (#include below this line is not maintained by Processor Expert) */
#include <stdio.h>
#include <string.h>
#include "Application.h"
#include "qca_spi.h"
#include "qca_block.h"

#include "libmme.h"
#include "libmme_over_spi.h"

LDD_TDeviceData *TimerData;
LDD_TDeviceData *spiMasterDevData;
LDD_TDeviceData *spiIntrDevData;

LDD_TDeviceData *Led1Data;    /* LED1 */
LDD_TDeviceData *Led2Data;    /* LED2 */
LDD_TDeviceData *Led3Data;    /* LED3 */
LDD_TDeviceData *Led4Data;    /* LED4 */

LDD_TDeviceData *Dip1_1Data;  /* DIP 1 Switch 1 */
LDD_TDeviceData *Dip1_2Data;  /* DIP 1 Switch 2 */
LDD_TDeviceData *Dip1_3Data;  /* DIP 1 Switch 3 */
LDD_TDeviceData *Dip1_4Data;  /* DIP 1 Switch 4 */
LDD_TDeviceData *Dip2_1Data;  /* DIP 2 Switch 1 */ 
LDD_TDeviceData *Dip2_2Data;  /* DIP 2 Switch 2 */
LDD_TDeviceData *Dip2_3Data;  /* DIP 2 Switch 3 */

LDD_TDeviceData *Con_X3_3Data; /* Ext Con X3 Pin 3 */
LDD_TDeviceData *Con_X3_4Data; /* Ext Con X3 Pin 4 */
LDD_TDeviceData *Con_X3_5Data; /* Ext Con X3 Pin 5 */
LDD_TDeviceData *Con_X3_6Data; /* Ext Con X3 Pin 6 */
LDD_TDeviceData *Con_X3_7Data; /* Ext Con X3 Pin 7 */
LDD_TDeviceData *Con_X3_8Data; /* Ext Con X3 Pin 8 */

QCASPI_DEVICE qca;             // SPI driver
uint8_t sendMme;               // request counter for MME broadcast
uint8_t qcaTimeout;            // counter flag for SPI driver timeout (50 ms)
uint8_t  led2Duration;         // counter for LED2 ( 0 = LED off, >0 = LED on)
uint16_t validMmeDuration;     // counter for LED4 ( 0 = LED off, >0 = LED on)
uint8_t outputGpioStates;      // output GPIO levels of Con X3
uint8_t newGpioDirections;     // input / output directions of GPIO on Con X3 = DIP switch settings 

uint8_t *receive_data; 
 uint8_t receivebuff[100];


/**
 *   This function retrieves the first 6 DIP switch settings.
 *   
 *   Bit 0 = DIP1_1 ... Bit 5 = DIP2_2  
 *   
 *   @brief function to retrieve the first 6 DIP switch settings
 *   @return first 6 DIP switch settings as flags (0 = off, 1 = on)
 */

////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

//Declaration of my written functions
static void SendChar(unsigned char ch, UART_Desc *desc);
static void SendString(const unsigned char *str,  UART_Desc *desc);


 typedef void LDD_TDeviceData; 
 typedef unsigned char uint8_t ;
 static UART_Desc deviceData;

  uint8_t buff[100];
 uint8_t collectbuff[100];
 uint8_t crc;
 uint8_t CRC7_POLY=0x91;
static uint8_t beefer[]="\r\n";
int counter;
uint8_t mybuff[100];
uint8_t rev[100];
uint8_t nana[100];
 int d=0;

 static uint8_t mme[] = {
 		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ODA              [0,5]
 		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // OSA (variable)   [6,11] 
 		0x88, 0xE1,                         // MTYPE            [12,13]
 		0x01,                               // MMV              [14]
 		0x02, 0xA0,                         // MMTYPE           [15,16]
 		0x00, 0x00,                         // FMI              [17,18]
 		0x00, 0x01, 0x87,                   // OUI              [19,21]
 		0x00,                               // MME_SUBVER       [22]
 		0x00,                               // STATE (variable) [23]
 		0x00,                               // DIR (variable)   [24]
 		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /////////////////////////////////My Stored data [25 to 140]
 		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
 		0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 		0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 		0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 		0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 		0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 		0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 		0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 		0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 		0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 		0x00, 0x00, 0x00, 0x00, 0x00 ,0x00,
 		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
 		0x00, 0x00, 0x00, 0x00, 0x00, 0x00,                 
 		0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 		0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 		0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
 		0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 		0x00                                 // My Stored data and Padding   [140]
 	};
/////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

uint8_t getGpioDirections(void)
{
	extern LDD_TDeviceData *Dip1_1Data;
	extern LDD_TDeviceData *Dip1_2Data;
	extern LDD_TDeviceData *Dip1_3Data;
	extern LDD_TDeviceData *Dip1_4Data;
	extern LDD_TDeviceData *Dip2_1Data;
	extern LDD_TDeviceData *Dip2_2Data;
	uint8_t result = 0;
		
	/* compress the values of the first 6 dip switches in a byte */
	result  =    ( DIP1_1_GetFieldValue(Dip1_1Data, Dip1_1Pin) << 0) 
			   | ( DIP1_2_GetFieldValue(Dip1_2Data, Dip1_2Pin) << 1)
			   | ( DIP1_3_GetFieldValue(Dip1_3Data, Dip1_3Pin) << 2)
			   | ( DIP1_4_GetFieldValue(Dip1_4Data, Dip1_4Pin) << 3)
			   | ( DIP2_1_GetFieldValue(Dip2_1Data, Dip2_1Pin) << 4)
			   | ( DIP2_2_GetFieldValue(Dip2_2Data, Dip2_2Pin) << 5);
	          
	
	/* negate all bits (DIP switches are active low) */
	result = ~result;
	
	return result;
}

/**
 *   This function retrieves the last 6 levels of GPIO input pins ().
 *   
 *   Bit 0 = CON_X3_3 ... Bit 5 = CON_X3_8
 *   
 *   @brief function to retrieve the levels of GPIO input pins
 *   @param gpioDirections GPIO directions of the connector pins (0 = input / 1 = output)
 *   @return last 6 levels of GPIO input pins as flags (0 = low, 1 = high)
 */
uint8_t getInputGpioStates(uint8_t gpioDirections)
{
	uint8_t result = 0;
			
	/* compress the values of the last 6 connector pins in a byte */
	if ((gpioDirections & 1) == 0) // Is CON_X3_3 input pin?
	{
		result |= CON_X3_3_GetFieldValue(Con_X3_3Data, Con_X3_3Pin) << 0;
	}
	
	if ((gpioDirections & 2) == 0) // Is CON_X3_4 input pin?
	{
		result |= CON_X3_4_GetFieldValue(Con_X3_4Data, Con_X3_4Pin) << 1;
	}
	
	if ((gpioDirections & 4) == 0) // Is CON_X3_5 input pin?
	{
		result |= CON_X3_5_GetFieldValue(Con_X3_5Data, Con_X3_5Pin) << 2;
	}
	
	if ((gpioDirections & 8) == 0) // Is CON_X3_6 input pin?
	{
		result |= CON_X3_6_GetFieldValue(Con_X3_6Data, Con_X3_6Pin) << 3;
	}
	
	if ((gpioDirections & 16) == 0) // Is CON_X3_7 input pin?
	{
		result |= CON_X3_7_GetFieldValue(Con_X3_7Data, Con_X3_7Pin) << 4;
	}
	
	if ((gpioDirections & 32) == 0) // Is CON_X3_8 input pin?
	{
		result |= CON_X3_8_GetFieldValue(Con_X3_8Data, Con_X3_8Pin) << 5;
	}
			  
	return result;
}

/**
 *   This function sets the GPIO directions and output level of the last 6 connector pins.
 *   
 *   Bit 0 = CON_X3_3 ... Bit 5 = CON_X3_8
 *   
 *   @brief function to set the GPIO directions and output level of the last 6 connector pins
 *   @param gpioDirections GPIO directions of the last 6 connector pins (0 = input / 1 = output)
 *   @param gpioStates last 6 levels for the GPIO output pins as flags (0 = low, 1 = high)
 */
void setGpios(uint8_t gpioDirections, uint8_t gpioStates)
{
	extern LDD_TDeviceData *Con_X3_3Data;
	extern LDD_TDeviceData *Con_X3_4Data;
	extern LDD_TDeviceData *Con_X3_5Data;
	extern LDD_TDeviceData *Con_X3_6Data;
	extern LDD_TDeviceData *Con_X3_7Data;
	extern LDD_TDeviceData *Con_X3_8Data;
	
	if (gpioDirections & 1) // Is CON_X3_3 output pin?
	{
		CON_X3_3_SetFieldOutputDirection(Con_X3_3Data, Con_X3_3Pin, (gpioStates & 1) >> 0);
	}
	else
	{
		CON_X3_3_SetFieldInputDirection(Con_X3_3Data, Con_X3_3Pin);
	}
	
	if (gpioDirections & 2) // Is CON_X3_4 output pin?
	{
		CON_X3_4_SetFieldOutputDirection(Con_X3_4Data, Con_X3_4Pin, (gpioStates & 2) >> 1);
	}
	else
	{
		CON_X3_4_SetFieldInputDirection(Con_X3_4Data, Con_X3_4Pin);
	}
	
	if (gpioDirections & 4) // Is CON_X3_5 output pin?
	{
		CON_X3_5_SetFieldOutputDirection(Con_X3_5Data, Con_X3_5Pin, (gpioStates & 4) >> 2);
	}
	else
	{
		CON_X3_5_SetFieldInputDirection(Con_X3_5Data, Con_X3_5Pin);
	}
	
	if (gpioDirections & 8) // Is CON_X3_6 output pin?
	{
		CON_X3_6_SetFieldOutputDirection(Con_X3_6Data, Con_X3_6Pin, (gpioStates & 8) >> 3);
	}
	else
	{
		CON_X3_6_SetFieldInputDirection(Con_X3_6Data, Con_X3_6Pin);
	}
	
	if (gpioDirections & 16) // Is CON_X3_7 output pin?
	{
		CON_X3_7_SetFieldOutputDirection(Con_X3_7Data, Con_X3_7Pin, (gpioStates & 16) >> 4);
	}
	else
	{
		CON_X3_7_SetFieldInputDirection(Con_X3_7Data, Con_X3_7Pin);
	}
	
	if (gpioDirections & 32) // Is CON_X3_8 output pin?
	{
		CON_X3_8_SetFieldOutputDirection(Con_X3_8Data, Con_X3_8Pin, (gpioStates & 32) >> 5);
	}
	else
	{
		CON_X3_8_SetFieldInputDirection(Con_X3_8Data, Con_X3_8Pin);
	}
}

/**
 *   This function sends a I2SE GPIO indication MME.  
 * 
 *   @brief function to send a I2SE GPIO indication MME
 *   @param libmme_config libmme structure that stores all internal settings 
 *   @param oda original destination address (broadcast MAC)
 *   @param osa original source address (your MAC)
 *   @param gpioDirections GPIO directions of the last 6 connector pins (0 = input / 1 = output)
 *   @param gpioStates last 6 levels of the GPIO pins as flags (0 = low, 1 = high)
 *   @retval >=0 the return value of libmme_over_spi_if_output function
 */
int send_i2se_gpio_indication(struct libmme* libmme_config, uint8_t oda[LIBMME_MAC_ADDRESS_SIZE], uint8_t osa[LIBMME_MAC_ADDRESS_SIZE],
		uint8_t gpioDirections, uint8_t gpioStates)
{	
	memcpy(&mme[0], oda, LIBMME_MAC_ADDRESS_SIZE); // set original destination address
	memcpy(&mme[6], osa, LIBMME_MAC_ADDRESS_SIZE); // set original source address
	mme[23] = 0x3F & gpioStates;                   // set GPIO states 
	mme[24] = 0xC0 | gpioDirections;               // set GPIO directions
	
	////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////
	//Splitting the uin16_t of the getUartData to character to fit into the
	//mme array structure and it will be sent to the other greenPHYs through the i2se
	
	//memcpy(&mme[33],beefer,15);
	
	
	/////////////////////////////////////////////////////////
   struct libmme_buffer output_data;
		
	
	output_data.data=mme;
	output_data.next=sizeof(mme);
	output_data.length=sizeof(mme);
	
	
	return libmme_over_spi_if_output(libmme_config, &output_data); // send MME over SPI to QCA7K
}

/**
 *   This function calculates the new GPIO states e.g. from a MME and sets them 
 * 
 *   @brief function to calculate the new GPIO states and set them
 *   @param senderGpioDirections GPIO directions of the sender
 *   @param senderGpioStates GPIO of the sender
 *   @param receiverGpioDirections GPIO directions of the receiver
 *   @param receiverGpioStates GPIO states of the receiver
 *   @return calculated GPIO states for the receiver
 */
uint8_t new_output_states(uint8_t senderGpioDirections, uint8_t senderGpioStates, uint8_t receiverGpioDirections, uint8_t receiverGpioStates)
{
	uint8_t result = (~senderGpioDirections & senderGpioStates) | (senderGpioDirections & receiverGpioStates); // calculate new output states
	setGpios(receiverGpioDirections, result);
	return result;
}

/**
 *   This function is a replacement for libmme_over_spi_if_input to handle MMEs
 *   direct without libmme.
 *   
 *   @brief function to handle MMEs direct
 *   @param netif libmme structure that stores all internal settings
 *   @retval #LIBMME_OVER_SPI_RESULT_OK when MME is handled or frame is ignored 
 *   @retval #LIBMME_OVER_SPI_RESULT_NULL_POINTER when netif is NULL
 *   @retval #LIBMME_OVER_SPI_RESULT_RUNTIME_ERROR when actually there is not input frame
 *   @retval #LIBMME_OVER_SPI_RESULT_INVALID_PARAM when input frame is invalid
 */
uint8_t mme_input(void *netif)
{
  struct libmme* libmme_config = (struct libmme*) netif;
  uint16_t receive_length = 0;
   
  extern uint8_t outputGpioStates, newGpioDirections;
  
  if (netif == NULL)
  {
	  return LIBMME_OVER_SPI_RESULT_NULL_POINTER;
  }
  
  receive_data = qcaspi_get_input_frame((QCASPI_DEVICE*)libmme_config->low_level_interface, &receive_length);
  if (receive_data == NULL)
  {
	  return LIBMME_OVER_SPI_RESULT_RUNTIME_ERROR;
  }
  
  if (receive_length < 32) // check length
  { 
	  return LIBMME_OVER_SPI_RESULT_OK; // ignore frame, too small length
  }
 
  if ((receive_data[12] != 0x88) ||
	  (receive_data[13] != 0xE1)) // check MTYPE
  {
	  return LIBMME_OVER_SPI_RESULT_OK; // ignore frame, unknown MTYPE
  }
  
  if (receive_data[14] != 0x01) // check MMV
  {
	  return LIBMME_OVER_SPI_RESULT_OK; // ignore frame, unknown MMV
  }
  
  if ((receive_data[15] != 0x02) ||
	  (receive_data[16] != 0xA0)) // check MMTYPE
  {
	  return LIBMME_OVER_SPI_RESULT_OK; // ignore frame, unknown MMTYPE
  }
 
  // ignore FMI
  
  if ((receive_data[19] != 0x00) ||
	  (receive_data[20] != 0x01) ||
	  (receive_data[21] != 0x87)) // check I2SE OUI
  {
	  return LIBMME_OVER_SPI_RESULT_OK; // ignore frame, unknown OUI
  }
  
  if (receive_data[22] != 0x00) // check MME_SUBVER
  {
	  return LIBMME_OVER_SPI_RESULT_INVALID_PARAM; // invalid MME_SUBVER
  }
  
  if (receive_data[23] & 0xC0) // check STATE
  {
	  return LIBMME_OVER_SPI_RESULT_INVALID_PARAM; // invalid STATE
  }
  
  if ((receive_data[24] & 0xC0) != 0xC0) // check DIR
  {
	  return LIBMME_OVER_SPI_RESULT_INVALID_PARAM; // invalid DIR
  }
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //Uncomment the code "memcpy(collectbuff,&receive_data[33],53" bellow for master greenPHY operation before flashing
 
  memcpy(collectbuff,&receive_data[33],100);
  
  //DONT DELETE THIS PART IS FOR THE SLAVE GREENPHY
 // memcpy(&mme[33],&receive_data[33],100);
  
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  outputGpioStates = new_output_states(receive_data[24], receive_data[23], newGpioDirections, outputGpioStates); // calculate und set new states
  validMmeDuration = 1; // switch LED4 on
 
  return LIBMME_OVER_SPI_RESULT_OK;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

static void SendChar(unsigned char ch, UART_Desc *desc) {
  desc->isSent = FALSE;  /* this will be set to 1 once the block has been sent */
  while(AS1_SendBlock(desc->handle, (LDD_TData*)&ch, 1)!=ERR_OK) {} /* Send char */
  while(!desc->isSent) {} /* wait until we get the green flag from the TX interrupt */
}
 
static void SendString(const unsigned char *str,  UART_Desc *desc) {
  while(*str!='\0') {
    SendChar(*str++, desc);
  }
}
 
static void Init(void) {
  /* initialize struct fields */
  deviceData.handle = AS1_Init(&deviceData);
  deviceData.isSent = FALSE;
  deviceData.rxChar = '\0';
  deviceData.rxPutFct = RxBuf_Put;
  /* set up to receive RX into input buffer */
  RxBuf_Init(); /* initialize RX buffer */
  /* Set up ReceiveBlock() with a single byte buffer. We will be called in OnBlockReceived() event. */
  while(AS1_ReceiveBlock(deviceData.handle, (LDD_TData *)&deviceData.rxChar, sizeof(deviceData.rxChar))!=ERR_OK) {
	  
  } /* initial kick off for receiving data */

}
 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void UHART(void){
if (RxBuf_NofElements()!=0) {
		      SendString((unsigned char*)"You Entered: ", &deviceData);
		      while (RxBuf_NofElements()!=0) {
		    	  unsigned char ch;
		 		        (void)RxBuf_Get(&ch);      
		        SendChar(ch, &deviceData);
		      }
  SendString((unsigned char*)"\r\n", &deviceData);
		    }

}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*To store the data
in a buffer*/
//////////////////////////////////////////////////////////

void storeData(void){
static int i=0;
//int g=0;

	unsigned char mych;
	
if (RxBuf_NofElements()!=0) {
	 SendString((unsigned char*)"Stored data::::: ", &deviceData);
	     
	 i = 0;
	 while ((RxBuf_NofElements()) !=0) 
		 {
			   (void)RxBuf_Get(&mych);
			   UTIL1_chcat(buff,sizeof buff, mych);
			 i++;	
			 
			 for (d = 0; d < 4000;)
			 		  {
			 			  d++;
			 		  }
		 }
	 memcpy(&buff[i],beefer,4);
	 memset(nana,'\0',sizeof nana);
	 memcpy(&mme[33],buff, 100);
	 memcpy(nana,&mme[33],100);
	 
	 }

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*To reverse stored data
in the buffer
*///////


void revData(void){
	int i=0;
	unsigned char ch;
	 int p=-1;
	 static int q;  
	 
	 
if (RxBuf_NofElements()!=0) {
	 SendString((unsigned char*)"Reversed data is:::::::::::::::::::::::::::::::::::::::::::::::::: ", &deviceData);
	 
	 i=0;
	     while (RxBuf_NofElements() !=0) 
		      {
	    	
		    	  (void)RxBuf_Get(&ch);
		    	  buff[i]= ch;
		    	  i++;  
		      }
	     while(buff[++p]!='\0');

	         while(p>=0){
	          rev[q++] = buff[--p];
	         }
	        rev[q]='\0';
	         
	      //  memcpy(&rev[q],beefer,4);
	        SendString(rev, &deviceData);
	        SendString((unsigned char*)"\r\n", &deviceData);
}	
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* To delay the loop
*/
void delay(void){
	
	for (d = 0; d < 4000;)
			  {
				  d++;
			  }
		}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* To perform CRC
on the Buff data
*//////////////////////////////////////////////////////////////////////////////////////////////////////////

unsigned char getCRC(unsigned char message[])
{
	
  unsigned char i, j;
  unsigned char length;
  length =sizeof (message);
 
  for (i = 0; i < length; i++)
  {
    crc ^= message[i];
    for (j = 0; j < 8; j++)
    {
      if (crc & 1)
        crc ^= CRC7_POLY;
      crc >>= 1;
    }
  }
  return crc;
}
	        

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/************************************************************************************
 *
 *   @brief function to enable all interrupts
 *
 ************************************************************************************/
void enableInt(void)
{
	__EI();
}

/************************************************************************************
 *
 *   @brief function to disable all interrupts
 *
 ************************************************************************************/
void disableInt(void)
{
	__DI();
} 



/*lint -save  -e970 Disable MISRA rule (6.3) checking. */
int main(void)
/*lint -restore Enable MISRA rule (6.3) checking. */
{
	uint8_t originalDstAddr[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }; // broadcast address
		
	/* Please replace with MAC hardware address MK20 on the PLC-Stamp 1 label. */
	uint8_t originalSrcAddr[6] = { 0x00, 0x01, 0x87, 0x04, 0x01, 0x91 };

	uint32_t ticks;
	uint8_t i;
	uint8_t newInputGpioStates, oldInputGpioStates, oldGpioDirections;
	struct libmme libmme_config;
	int result;
	
	////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////
	
	 
    ///////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////
     
	/*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
	PE_low_level_init();
	/*** End of Processor Expert internal initialization.                    ***/
	
	/* Write your code here */
	/* For example: for(;;) { } */
	
	/* Init all 4 LEDs */
	Led1Data = LED1_Init(NULL);
	Led2Data = LED2_Init(NULL);
	Led3Data = LED3_Init(NULL);
	Led4Data = LED4_Init(NULL);
	
	/* Init the 2 DIPs */
	Dip1_1Data = DIP1_1_Init(NULL);
	Dip1_2Data = DIP1_2_Init(NULL);
	Dip1_3Data = DIP1_3_Init(NULL);
	Dip1_4Data = DIP1_4_Init(NULL);
	Dip2_1Data = DIP2_1_Init(NULL);
	Dip2_2Data = DIP2_2_Init(NULL);
	Dip2_3Data = DIP2_3_Init(NULL);
	
	/* Init GPIOs on external connector X3 */
	Con_X3_3Data = CON_X3_3_Init(NULL);
	Con_X3_4Data = CON_X3_4_Init(NULL);
	Con_X3_5Data = CON_X3_5_Init(NULL);
	Con_X3_6Data = CON_X3_6_Init(NULL);
	Con_X3_7Data = CON_X3_7_Init(NULL);
	Con_X3_8Data = CON_X3_8_Init(NULL);
	
	spiMasterDevData = SM1_Init(&qca);    // Init spi master interface with pointer to the driver
	spiIntrDevData = SPI_INTR_Init(&qca); // Init spi interrupt gpio with pointer to the driver
	
	///////////////////////////////////////////
	//Initialising the UART
	 Init();                                            
	///////////////////////////////////
	
	/* Init SPI driver with attribute set 0 (default: Width = 8 bit, MSB first, CPOL=1, CPHA=1, No parity, No CS toggle) 
	 * and chip 0 (chip select = active low) */
	qcaspi_init(&qca,
				  spiMasterDevData,         // spi master device
				  spiIntrDevData,           // spi interrupt gpio device
				  SpiIntr,                  // spi interrupt gpio number
				  SM1_ReceiveBlock,         // pointer to spi receive function 
				  SM1_SendBlock,            // pointer to spi send function
				  SM1_SelectConfiguration,  // pointer to spi config function
				  SPI_INTR_GetFieldValue,   // pointer to get level of spi interrupt
				  disableInt,               // pointer to disable all interrupts
				  enableInt);               // pointer to enable all interrupts
	
	libmme_init(&libmme_config, &qca, libmme_over_spi_if_output, NULL, NULL);
	libmme_over_spi_if_init(&libmme_config);
		
	qcaspi_set_interface(&qca, &libmme_config);
	qcaspi_set_interface_input(&qca, mme_input); // use custom frame input function instead from libmme_over_spi
	
	for (i = 0; i < 3; i++)
	{
	  /* Switch all LEDs on */
	  LED1_ClearFieldBits(Led1Data, Led1Pin, 1);
	  LED2_ClearFieldBits(Led2Data, Led2Pin, 1);
	  LED3_ClearFieldBits(Led3Data, Led3Pin, 1);
	  LED4_ClearFieldBits(Led4Data, Led4Pin, 1);
	  
	  for (ticks = 0; ticks < 4000000;)
	  {
		  ticks++;
	  }
	  
	  /* Switch all LEDs off */
	  LED1_SetFieldBits(Led1Data, Led1Pin, 1);
	  LED2_SetFieldBits(Led2Data, Led2Pin, 1);
	  LED3_SetFieldBits(Led3Data, Led3Pin, 1);
	  LED4_SetFieldBits(Led4Data, Led4Pin, 1);
	  
	  for (ticks = 0; ticks < 4000000;)
	  {
		  ticks++;
	  }
	}
	
	/* Start timer (interval = 50 ms) */
	TimerData = TIMER_Init(&qca);
	
	newGpioDirections = getGpioDirections();
	newInputGpioStates = getInputGpioStates(newGpioDirections);
	
	/* Init GPIO directions and states */
	if (DIP2_3_GetFieldValue(Dip2_3Data, Dip2_3Pin) > 0)
	{
		outputGpioStates = 0x00; // Inital low level
	}
	else
	{
		outputGpioStates = 0x3F; // Inital high level
	}
	
	setGpios(newGpioDirections, outputGpioStates);
	oldGpioDirections = newGpioDirections;
	oldInputGpioStates = newInputGpioStates;
	
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	
			SendString((unsigned char*)"Welcome\r\n", &deviceData);
			 
	 //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			 
	/* application main loop */
	
	while (1)
	{
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/////////////////// Data transfer section to distingush between master and slave /////////////////////////
		//////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* Comment out the storeData() leaving the UHART() funtion to run for the master module
Comment out the UHART function leaving the storeDAta() function to run for the slave module*/
									
		UHART();
		
		storeData();

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/* poll every 50 ms DIP switches and GPIOs */
		if (qcaTimeout > 0)
		{
			qcaTimeout--;
			newGpioDirections  = getGpioDirections();
			newInputGpioStates = getInputGpioStates(newGpioDirections);
			
			if ((newGpioDirections  != oldGpioDirections) ||
				(newInputGpioStates != oldInputGpioStates)) // changes?
			{
				setGpios(newGpioDirections, outputGpioStates);
				oldGpioDirections  = newGpioDirections;
				oldInputGpioStates = newInputGpioStates;
				sendMme++;
			}
		}
		
		if (qcaspi_is_sync(&qca))
		{
			/* If synchronisation is ready, switch on LED3 */
			LED3_ClearFieldBits(Led3Data, Led3Pin, 1);
			
		}
		else  
		{
			LED3_SetFieldBits(Led3Data, Led3Pin, 1); 
			
		}
			  
		(void) qcaspi_process(&qca); // async spi task
		
		if ((sendMme > 0) && 
			qcaspi_is_sync(&qca))
		{
			result = send_i2se_gpio_indication(&libmme_config,originalDstAddr,originalSrcAddr,newGpioDirections,(~newGpioDirections & newInputGpioStates) | (newGpioDirections & outputGpioStates)); // broadcast MME	
				
			
			delay();
			
			SendString(collectbuff, &deviceData);
			

			memset(collectbuff,0, 100);
			memset(&mme[33],0,100);
			
			if (result == LIBMME_OVER_SPI_RESULT_OK)
			{
				sendMme--;
				led2Duration = 1; // switch LED2 on
			}
		}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* To clear all Buffer*/		
////////////////////////////////////////////////////////////////////////////////////////////////////////		
		int i=0;
		
		for(i=0;i<sizeof(buff);){
			buff[i]='\0';
			nana[i]='\0';
			//collectbuff[i]='\0';
			//rev[i]='\0';
			i++;
		}
	
		
					   				
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////			
	}
	
  /*** Don't write any code pass this line, or it will be deleted during code generation. ***/
  /*** RTOS startup code. Macro PEX_RTOS_START is defined by the RTOS component. DON'T MODIFY THIS CODE!!! ***/
  #ifdef PEX_RTOS_START
    PEX_RTOS_START();                  /* Startup of the selected RTOS. Macro is defined by the RTOS component. */
  #endif
  /*** End of RTOS startup code.  ***/
  /*** Processor Expert end of main routine. DON'T MODIFY THIS CODE!!! ***/
  for(;;){}
  /*** Processor Expert end of main routine. DON'T WRITE CODE BELOW!!! ***/
} /*** End of main routine. DO NOT MODIFY THIS TEXT!!! ***/

/* END ProcessorExpert */
/*
** ###################################################################
**
**     This file was created by Processor Expert 10.0 [05.03]
**     for the Freescale Kinetis series of microcontrollers.
**
** ###################################################################
*/

