/******************************************************************************
 *
 * Module: Basic Communication Modul (BCM) service layer
 *
 * File Name: BCM.h
 *
 * Description: Header file for BCM Driver
 *
 * Created on: Sep 13, 2019
 *
 * Author: Ahmed Eldakhly & Hesham Hafez
 *
 *******************************************************************************/

#ifndef BCM_H_
#define BCM_H_

/*******************************************************************************
 *                       	Included Libraries                                 *
 *******************************************************************************/
#include "UART.h"
#include "CPU_sleep.h"
#include "BCM_Cfg.h"

/*******************************************************************************
 *                         Types Declaration                                   *
 *******************************************************************************/

/************************ EnmBCM_TxDispatch_t **********************************/
typedef enum
{
	TX_IDLE,
	SENDING_FRAME_IN_PROGRESS,
	SENDING_Byte_IN_PROGRESS
}EnmBCM_TxDispatchStatus_t;

/***************************** EnmBCM_RxDispatch_t *****************************/
typedef enum
{
	RX_IDLE,
	RECEIVING_FRAME_IN_PROGRESS,
	RECEIVING_Byte_IN_PROGRESS
}EnmBCM_RxDispatchStatus_t;

/************************ EnmBCM_SendingFrameStatus_t **************************/
typedef enum
{
	SENDING_SIZE,
	SENDING_DATA,
	SENDING_CHECKSUM,
	END_SENDING_FRAME
}EnmBCM_SendingFrameStatus_t;

/************************ EnmBCM_ReceivingFrameStatus_t ***********************/
typedef enum
{
	RECEIVING_SIZE,
	RECEIVING_DATA,
	RECEIVING_CHECKSUM
}EnmBCM_ReceivingFrameStatus_t;

/****************************** EnmBCMstatus_t ********************************/
typedef enum{
	BCM_OK,
	BCM_NOT_OK,
	BCM_BUSY,
	BCM_SEND_COMPLETE,
	BCM_SEND_FAIL,
	BCM_RECEIVE_COMPLETE,
	BCM_RECEIVE_FAIL
}EnmBCMstatus_t;

/**************************** StrBCM_BufferInfo_t ****************************/
typedef struct
{
	uint16 * ArraySize;
	uint8 * DataPtr;
	uint8 command;
}StrBCM_RxBufferInfo_t;

/**************************** StrBCM_BufferInfo_t ****************************/
typedef struct
{
	uint8 command;
	uint16 ArraySize;
	uint8 * DataPtr;
}StrBCM_TxBufferInfo_t;

/**************************** StrBCM_TxInfo_t ****************************/
typedef struct
{
	uint8 Lock;
	uint16 Index;
	sint16 FailIndex;
	uint8  CheckSum;
}StrBCM_TxInfo_t;

/**************************** StrBCM_RxInfo_t ****************************/
typedef struct
{
	uint8 Lock;
	uint16 Index;
	sint16 FailIndex;
	uint8  CheckSum;
}StrBCM_RxInfo_t;

/*******************************************************************************
 *                      Functions Prototypes                                   *
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
extern EnmBCMstatus_t BCM_Init(void);

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
extern EnmBCMstatus_t BCM_Send(uint8 * a_Data_Ptr, uint16 a_size);

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
extern EnmBCMstatus_t BCM_TxDispatch(void);

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
extern EnmBCMstatus_t BCM_SetupReceive(uint8 * a_Data_Ptr, uint16 * a_size);

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
extern EnmBCMstatus_t BCM_RxDispatch(void);

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
extern EnmBCM_RxDispatchStatus_t BCM_Receive(void);

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
extern EnmBCMstatus_t BCM_DeInit(void);


#endif /* BCM_H_ */
