/******************************************************************************
 *
 * Module: Basic Communication Module (BCM) service layer
 *
 * File Name: BCM.c
 *
 * Description: Source file for BCM Driver
 *
 * Created on: Sep 13, 2019
 *
 * Author: Ahmed Eldakhly & Hesham Hafez
 *
 *******************************************************************************/


/*******************************************************************************
 *                       	Included Libraries                                 *
 *******************************************************************************/
#include "BCM.h"

/*******************************************************************************
 *                           Global Variables                                  *
 *******************************************************************************/
/*BCM Tx and Rx data Buffer*/
static StrBCM_TxBufferInfo_t BCM_Tx_Buffer = {0u,0u,NULL_PTR};
static StrBCM_RxBufferInfo_t BCM_Rx_Buffer = {NULL_PTR,NULL_PTR,0u};

/*BCM Data information*/
static StrBCM_TxInfo_t BCM_Tx_Info = {0u,0u,0u};
static StrBCM_RxInfo_t BCM_Rx_Info = {0u,0u,0u};

/*BCM Dispatch Tx and Rx status*/
static EnmBCM_TxDispatchStatus_t Dispatch_TX_Status = TX_IDLE;
static EnmBCM_RxDispatchStatus_t Dispatch_RX_Status = RX_IDLE;

Status_t UART_InterruptStatus = NOT_OK;

/*BCM frame Status*/
static EnmBCM_ReceivingFrameStatus_t ReceivingFrameStatus = RECEIVING_SIZE;

/*To hold the value of the 16 bit size on two steps
and add them on two stages to the total data size in the header*/
static uint8 ReceiveSizeBuffer = 0u;

/*******************************************************************************
 *                       Static function prototype	                           *
 *******************************************************************************/
static void UART_TxCallbackFunction(void);
static void UART_RxCallbackFunction(void);

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/

/*******************************************************************************
 * Function Name:	BCM_Init
 *
 * Description: 	Initialize the BCM Driver
 *
 * Inputs:			NULL
 *
 * Outputs:			NULL
 *
 * Return:			Status to check function execution
 *******************************************************************************/
EnmBCMstatus_t BCM_Init(void)
{
	/*return value*/
	EnmBCMstatus_t ReturnVal;
	/*initialization of UART communication protocol*/
	Status_t UART_Init_check =	UART_Init();
	/*if initialization succeeded*/
	if(UART_Init_check == OK)
	{
		ReturnVal = BCM_OK;
	}
	/*if initialization failed*/
	else
	{
		ReturnVal = BCM_NOT_OK;
	}
	/*start of UART communication protocol*/
	UART_Init_check = UART_StartComm();
	/*if UART start succeeded*/
	if(UART_Init_check == OK)
	{
		ReturnVal = BCM_OK;
	}
	/*if start UART failed*/
	else
	{
		ReturnVal = BCM_NOT_OK;
	}
	if(ReturnVal ==  BCM_OK)
	{
		/*TX Data Buffer*/
		BCM_Tx_Buffer.DataPtr = NULL_PTR;

		/*RX Data Buffer*/
		BCM_Rx_Buffer.DataPtr = NULL_PTR;

		/*Initialize Tx checksum value*/
		BCM_Tx_Info.CheckSum = 0u;

		/*Initialize Tx Array Size value*/
		BCM_Tx_Buffer.ArraySize = 0u;

		/*Initialize Tx Index value*/
		BCM_Tx_Info.Index = 0u;

		/*Initialize Tx FailIndex value*/
		BCM_Tx_Info.FailIndex = (sint16)(-1);

		/*Initialize Rx checksum value*/
		BCM_Rx_Info.CheckSum = 0u;

		/*Initialize Rx Array Size value*/
		BCM_Rx_Buffer.ArraySize = 0u;

		/*Initialize Rx Index value*/
		BCM_Rx_Info.Index = 0u;

		/*Initialize Rx FailIndex value*/
		BCM_Rx_Info.FailIndex = (sint16)(-1);

		/*Initialize Tx Dispatcher State*/
		Dispatch_TX_Status = TX_IDLE;

		/*Initialize Rx Dispatcher State*/
		Dispatch_RX_Status = RX_IDLE;

		/*Call Back Functions Initialization*/
		UART_Config.UART_Rx_CallBack_Function = UART_RxCallbackFunction;
		UART_Config.UART_Tx_CallBack_Function = UART_TxCallbackFunction;
	}
	else
	{
		/*Do Nothing*/
	}

	return ReturnVal;
}

/*******************************************************************************
 * Function Name:	BCM_Send
 *
 * Description: 	start the sending communication
 *
 * Inputs:			pointer to array	(uint8)
 * 					uint16 size of Data array
 *
 * Outputs:			NULL
 *
 * Return:			Status to check function execution
 *******************************************************************************/
EnmBCMstatus_t BCM_Send(uint8 * a_Data_Ptr, uint16 a_size)
{

	/*return value*/
	EnmBCMstatus_t ReturnVal;

	/*if BCM driver in idle mode*/
	if(Dispatch_TX_Status == TX_IDLE)
	{
		/*assign address of data array*/
		BCM_Tx_Buffer.DataPtr = a_Data_Ptr;
		/*assign size*/
		BCM_Tx_Buffer.ArraySize = a_size;

		/*initializing Tx header*/
		BCM_Tx_Info.Index = 0u;
		BCM_Tx_Info.CheckSum = 0u;
		BCM_Tx_Info.FailIndex = ((sint8)(-1));

		/*Changing to sending mode*/
		Dispatch_TX_Status = SENDING_FRAME_IN_PROGRESS;
		ReturnVal = BCM_OK;
	}
	else
	{
		ReturnVal = BCM_BUSY;
	}

	return ReturnVal;
}

/*******************************************************************************
 * Function Name:	BCM_TxDispatch
 *
 * Description: 	Check status for sending communication in BCM
 *
 * Inputs:			NULL
 *
 * Outputs:			NULL
 *
 * Return:			Status to check function execution
 *******************************************************************************/
EnmBCMstatus_t BCM_TxDispatch(void)
{
	Status_t UART_status = NOT_OK;
	static EnmBCM_SendingFrameStatus_t SendingFrameStatus = SENDING_SIZE;
	/*return value*/
	EnmBCMstatus_t ReturnVal;
	//	static EnmBCM_SendingFrameStatus_t SendingFrameStatus = SENDING_SIZE;
	/*switch between Tx Dispatch Modes*/
	switch (Dispatch_TX_Status)
	{
	/*Tx idle mode case*/
	case TX_IDLE:
		/*Do Nothing*/
		break;

		/*Sending Frame "Header and Data" Mode*/
	case SENDING_FRAME_IN_PROGRESS:
		/*switch between sending size or sending Data*/
		switch(SendingFrameStatus)
		{
		case SENDING_SIZE:
			/*check the index of the Tx info Buffer*/
			if(BCM_Tx_Info.Index == 0u)
			{
				/*send first byte of the size*/
				UART_status = UART_SendChar((BCM_Tx_Buffer.ArraySize) & (0x00ffu));
				/*check if sending data failed*/
				if(UART_status == NOT_OK)
				{
					BCM_Tx_Info.FailIndex = ((BCM_Tx_Info.Index));
					/*switch to Tx idle mode*/
					Dispatch_TX_Status = TX_IDLE;
					ReturnVal = BCM_SEND_FAIL;
				}

				else
				{
					/* Do nothing*/
				}

				(BCM_Tx_Info.Index)++;
			}

			else if(BCM_Tx_Info.Index == 1u)
			{
				/*send second byte of the size*/
				UART_status = UART_SendChar((((BCM_Tx_Buffer.ArraySize) & (0xff00u))>> 8u));

				/*check if sending data failed*/
				if(UART_status == NOT_OK)
				{
					BCM_Tx_Info.FailIndex = ((BCM_Tx_Info.Index));
					/*switch to Tx idle mode*/
					Dispatch_TX_Status = TX_IDLE;
					ReturnVal = BCM_SEND_FAIL;
				}

				else
				{
					/* Do nothing*/
				}

				/*Reset Index value*/
				BCM_Tx_Info.Index = 0u;

				/*switch to case : sending data */
				SendingFrameStatus = SENDING_DATA;
			}

			Dispatch_TX_Status = SENDING_Byte_IN_PROGRESS;
			break;

		case SENDING_DATA:

			/*calculate checksum*/
			BCM_Tx_Info.CheckSum += BCM_Tx_Buffer.DataPtr[BCM_Tx_Info.Index];
			BCM_Tx_Info.Index++;
			/*check if the index of Tx info buffer equal to array size*/
			if(BCM_Tx_Info.Index == ((BCM_Tx_Buffer.ArraySize)))
			{
				/*switch to case sending checksum*/
				SendingFrameStatus = SENDING_CHECKSUM;
			}

			/*sending data*/
			UART_status=UART_SendChar(*(BCM_Tx_Buffer.DataPtr + (BCM_Tx_Info.Index - 1u)));

			/*check if sending data failed*/
			if(UART_status == NOT_OK)
			{
				BCM_Tx_Info.FailIndex = ((BCM_Tx_Info.Index) - 1u);
				/*switch to Tx idle mode*/
				Dispatch_TX_Status = TX_IDLE;
				ReturnVal = BCM_SEND_FAIL;
			}

			else
			{
				/* Do nothing*/
			}
			/*switch to SENDING_Byte_IN_PROGRESS mode*/
			Dispatch_TX_Status = SENDING_Byte_IN_PROGRESS;
			break;

		case SENDING_CHECKSUM:

			/*sending checksum value*/
			UART_status = UART_SendChar(BCM_Tx_Info.CheckSum);

			/*check if sending Checksum failed*/
			if(UART_status == NOT_OK)
			{
				BCM_Tx_Info.FailIndex = ((BCM_Tx_Info.Index));
				/*switch to Tx idle mode*/
				Dispatch_TX_Status = TX_IDLE;
				ReturnVal = BCM_SEND_FAIL;
			}
			else
			{
				/* Do nothing*/
			}
			/*switch to end sending frame*/
			SendingFrameStatus = END_SENDING_FRAME;
			/*switch to SENDING_Byte_IN_PROGRESS mode*/
			Dispatch_TX_Status = SENDING_Byte_IN_PROGRESS;
			break;

		case END_SENDING_FRAME:
			/*switch to case : sending data */
			SendingFrameStatus = SENDING_SIZE;
			ReturnVal = BCM_SEND_COMPLETE;
			/*switch to Tx idle mode*/
			Dispatch_TX_Status = TX_IDLE;
			break;

		default:
			/*Do nothing*/
			break;
		}

		/*end of SENDING_FRAME_IN_PROGRESS case*/
		break;

		case SENDING_Byte_IN_PROGRESS:
			/*Do Nothing*/
			break;

		default:
			/*Do nothing*/
			break;
	}
	return ReturnVal;
}

/*******************************************************************************
 * Function Name:	BCM_SetupReceive
 *
 * Description: 	Start the receiving communication
 *
 * Inputs:			NULL
 *
 * Outputs:			pointer to array 		(uint8)
 * 					pointer to array size 	(uint16)
 *
 * Return:			Status to check function execution
 *******************************************************************************/
EnmBCMstatus_t BCM_SetupReceive(uint8 * a_Data_Ptr, uint16 * a_size)
{
	/*return value*/
	EnmBCMstatus_t ReturnVal;
	/*if BCM driver in idle mode*/
	if(Dispatch_RX_Status == RX_IDLE)
	{
		/*assign address of data array*/
		BCM_Rx_Buffer.DataPtr = a_Data_Ptr;

		/*assign address of the size of array*/
		BCM_Rx_Buffer.ArraySize =  a_size;

		/*initializing Rx header*/
		BCM_Rx_Info.Index = 0u;
		BCM_Rx_Info.CheckSum = 0u;
		BCM_Rx_Info.FailIndex = (sint8)(-1);

		/*Changing to receive mode*/
		Dispatch_RX_Status = RECEIVING_FRAME_IN_PROGRESS;
		ReturnVal = BCM_OK;
	}
	else
	{
		ReturnVal = BCM_BUSY;
	}

	return ReturnVal;
}

/*******************************************************************************
 * Function Name:	BCM_RxDispatch
 *
 * Description: 	Check status for receiving communication in BCM
 *
 * Inputs:			NULL
 *
 * Outputs:			NULL
 *
 * Return:			Status to check function execution
 *******************************************************************************/
EnmBCMstatus_t BCM_RxDispatch(void)
{
	/*return value*/
	EnmBCMstatus_t ReturnVal;
	/*Buffer to save the received size of the Data*/
	static uint16 u16ReceiveSizeBuffer = 0u;

	/*This value to be incremented while receiving bytes to compare it
	 *  with the checksum I receive at the end of the data stream*/
	static uint8 CheckSumCalculation = 0u;

	/*switch between Rx dispatch modes*/
	switch (Dispatch_RX_Status)
	{
	case RX_IDLE:
		/*Do Nothing*/
		break;

	case RECEIVING_FRAME_IN_PROGRESS:
		/*switch between Receiving size or Receiving Data*/
		switch(ReceivingFrameStatus)
		{
		case RECEIVING_SIZE:
			/*check the index of the Rx info Buffer*/
			if(BCM_Rx_Info.Index == 1u)
			{
				/*check if receiving data failed*/
				if(UART_InterruptStatus == NOT_OK)
				{
					/*switching to idle Rx  mode*/
					Dispatch_RX_Status = RX_IDLE;
					ReturnVal = BCM_RECEIVE_FAIL;
				}

				else
				{
					/* Do nothing*/
				}

				/*assign first 8 bit of size to the receive size buffer*/
				u16ReceiveSizeBuffer = ReceiveSizeBuffer;
			}

			/*check the index of the Rx info Buffer*/
			else if(BCM_Rx_Info.Index == 2u)
			{
				/*check if receiving data failed*/
				if(UART_InterruptStatus == NOT_OK)
				{
					/*switching to idle Rx  mode*/
					Dispatch_RX_Status = RX_IDLE;
					ReturnVal = BCM_RECEIVE_FAIL;
				}

				else
				{
					/* Do nothing*/
				}

				/*assign second 8 bit of size to the receive size buffer*/
				u16ReceiveSizeBuffer |= (((uint16)ReceiveSizeBuffer)<<8u);
				/*complete receiving buffer*/
				(*BCM_Rx_Buffer.ArraySize) = u16ReceiveSizeBuffer;

				/*Reset Index value*/
				BCM_Rx_Info.Index = 0u;

				/*switch to case : receiving data */
				ReceivingFrameStatus = RECEIVING_DATA;
			}

			/*switch to RECEIVING_Byte_IN_PROGRESS mode*/
			Dispatch_RX_Status = RECEIVING_Byte_IN_PROGRESS;
			break;

		case RECEIVING_DATA:
			/*check if receiving data failed*/
			if(UART_InterruptStatus == NOT_OK)
			{

				/*check the index of the failure*/
				BCM_Rx_Info.FailIndex = BCM_Rx_Info.Index - 1u;
				/*switching to idle Rx  mode*/
				Dispatch_RX_Status = RX_IDLE;
				ReturnVal = BCM_RECEIVE_FAIL;
			}

			else
			{
				/*calculate the checksum value*/
				CheckSumCalculation += BCM_Rx_Buffer.DataPtr[BCM_Rx_Info.Index - 1u];
			}

			/*check if the index of Rx info buffer equal to array size*/
			if((BCM_Rx_Info.Index) == ((*BCM_Rx_Buffer.ArraySize)))
			{
				/*switch to case receiving checksum*/
				ReceivingFrameStatus = RECEIVING_CHECKSUM;
			}

			/*switch to RECEIVING_Byte_IN_PROGRESS mode*/
			Dispatch_RX_Status = RECEIVING_Byte_IN_PROGRESS;
			break;

		case RECEIVING_CHECKSUM:

			/*check if receiving Checksum failed*/
			if(UART_InterruptStatus == NOT_OK)
			{
				/*check the index of the failure*/
				BCM_Rx_Info.FailIndex = *BCM_Rx_Buffer.ArraySize - 1u;

				/*switching to idle Rx idle mode*/
				Dispatch_RX_Status = RX_IDLE;
				ReturnVal = BCM_RECEIVE_FAIL;
			}
			else
			{
				/* Do nothing*/
			}

			/*if received checksum equal to calculated checksum*/
			if(BCM_Rx_Info.CheckSum == CheckSumCalculation)
			{
				ReturnVal = BCM_RECEIVE_COMPLETE;
			}

			else
			{

				ReturnVal = BCM_RECEIVE_FAIL;
			}
			/*switch to RECEIVING_SIZE case*/
			ReceivingFrameStatus = RECEIVING_SIZE;

			/*switch to Rx idle mode*/
			Dispatch_TX_Status = RX_IDLE;
			break;

		default:
			/*Do nothing*/
			break;
		}
		/*end of RECEIVING_FRAME_IN_PROGRESS case*/
		break;

		case RECEIVING_Byte_IN_PROGRESS:
			/*Do Nothing*/
			break;

		default:
			/*Do nothing*/
			break;
	}
	return ReturnVal;
}

/*******************************************************************************
 * Function Name:	BCM_Receive
 *
 * Description: 	return status of receiving to the user
 *
 * Inputs:			NULL
 *
 * Outputs:			NULL
 *
 * Return:			Status to check receiving status
 *******************************************************************************/
EnmBCM_RxDispatchStatus_t BCM_Receive(void)
{
	return Dispatch_RX_Status;
}

/*******************************************************************************
 * Function Name:	BCM_DeInit
 *
 * Description: 	Disable the BCM Driver
 *
 * Inputs:			NULL
 *
 * Outputs:			NULL
 *
 * Return:			Status to check function execution
 *******************************************************************************/
EnmBCMstatus_t BCM_DeInit(void)
{
	EnmBCMstatus_t ReturnVal;
	/*stop UART communication protocol*/
	Status_t UART_DeInit_check = UART_StopComm();
	if(UART_DeInit_check == OK)
	{
		ReturnVal = BCM_OK;
	}
	else
	{
		ReturnVal = BCM_NOT_OK;
	}
	/*reInitaization of all buffers*/
	BCM_Tx_Buffer.DataPtr = NULL_PTR;
	BCM_Rx_Buffer.DataPtr = NULL_PTR;
	BCM_Tx_Info.CheckSum = 0u;
	BCM_Tx_Buffer.ArraySize = 0u;
	BCM_Tx_Info.Index = 0u;
	BCM_Tx_Info.FailIndex = (sint16)(-1);
	BCM_Rx_Info.CheckSum = 0u;
	BCM_Rx_Buffer.ArraySize = 0u;
	BCM_Rx_Info.Index = 0u;
	BCM_Rx_Info.FailIndex = (sint16)(-1);
	Dispatch_TX_Status = TX_IDLE;
	Dispatch_RX_Status = RX_IDLE;
	return ReturnVal;
}

/*******************************************************************************
 * Function Name:	UART_TxCallbackFunction
 *
 * Description: 	static Callback function from UART_Tx interrupt to continue
 * 					sending of data
 *
 * Inputs:			NULL
 *
 * Outputs:			NULL
 *
 * Return:			NULL
 *******************************************************************************/
void UART_TxCallbackFunction(void)
{
	if(Dispatch_TX_Status == SENDING_Byte_IN_PROGRESS)
	{
		Dispatch_TX_Status = SENDING_FRAME_IN_PROGRESS;
	}
}

/*******************************************************************************
 * Function Name:	UART_RxCallbackFunction
 *
 * Description: 	static Callback function from UART_Rx interrupt to continue
 * 					receiving of data
 *
 * Inputs:			NULL
 *
 * Outputs:			NULL
 *
 * Return:			NULL
 *******************************************************************************/
void UART_RxCallbackFunction(void)
{
	if(Dispatch_RX_Status==RECEIVING_Byte_IN_PROGRESS)
	{
		/*receiving data*/
		switch(ReceivingFrameStatus)
		{
		case RECEIVING_SIZE:
			/*receive first byte of the size*/
			UART_InterruptStatus=UART_ReceiveChar(&ReceiveSizeBuffer);
			break;
		case RECEIVING_DATA:
			UART_InterruptStatus = UART_ReceiveChar((BCM_Rx_Buffer.DataPtr+BCM_Rx_Info.Index));
			break;
		case RECEIVING_CHECKSUM:
			/*receiving checksum value*/
			UART_InterruptStatus = UART_ReceiveChar(&(BCM_Rx_Info.CheckSum));
			break;
		default:
			/*DO nothing*/
			break;
		}
		/*return to RECEIVING_FRAME_IN_PROGRESS State*/
		Dispatch_RX_Status = RECEIVING_FRAME_IN_PROGRESS;
		BCM_Rx_Info.Index++;
	}
}





