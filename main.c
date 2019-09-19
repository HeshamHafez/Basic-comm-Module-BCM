/******************************************************************************
 *
 * Module: Basic Communication Module (BCM) main App
 *
 * File Name: main.c
 *
 * Description: main Application
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
#include <avr/delay.h>
#include <avr/interrupt.h>
#include "DIO.h"

/*******************************************************************************
 *                             Main Function                                   *
 *******************************************************************************/
int main(void)
{
	EnmBCMstatus_t BCM_Status;

	uint8 SendData_Ptr[] = "Ahmed El-Dakhly & Hesham Hafez want to say thank you Hossam for helping us, and this is for switch case :D 1234";
	uint16 SendSize = (sizeof(SendData_Ptr));

	uint16 x;
	uint16 * ReceiveSize = &x;
	uint8 ReceiveData_Ptr[MAX_DATA_RECEIVED]={0};

	BCM_Init();
	BCM_Send(SendData_Ptr, SendSize);
	BCM_SetupReceive(ReceiveData_Ptr , ReceiveSize);

	DIO_SetPinDirection(DIO_PIN12,HIGH);
	DIO_SetPinDirection(DIO_PIN13,HIGH);
	DIO_SetPinDirection(DIO_PIN14,HIGH);
	DIO_SetPinDirection(DIO_PIN15,HIGH);

	CPU_Sleep();
	SET_BIT(SREG,7);

	while(1)
	{
		BCM_Status = BCM_TxDispatch();
		BCM_Status = BCM_RxDispatch();

		if(BCM_Status == BCM_RECEIVE_COMPLETE)
		{
			CLEAR_BIT(SREG,I);
			for(uint8 i = 0; i<(*ReceiveSize); i++)
			{
				switch(ReceiveData_Ptr[i])
				{
				case '1':
					DIO_WritePin(DIO_PIN12,HIGH);
					DIO_WritePin(DIO_PIN13,LOW);
					DIO_WritePin(DIO_PIN14,LOW);
					DIO_WritePin(DIO_PIN15,LOW);
					_delay_ms(300);
					break;
				case '2':
					DIO_WritePin(DIO_PIN12,LOW);
					DIO_WritePin(DIO_PIN13,HIGH);
					DIO_WritePin(DIO_PIN14,LOW);
					DIO_WritePin(DIO_PIN15,LOW);
					_delay_ms(300);
					break;
				case '3':
					DIO_WritePin(DIO_PIN12,LOW);
					DIO_WritePin(DIO_PIN13,LOW);
					DIO_WritePin(DIO_PIN14,HIGH);
					DIO_WritePin(DIO_PIN15,LOW);
					_delay_ms(300);
					break;
				case '4':
					DIO_WritePin(DIO_PIN12,LOW);
					DIO_WritePin(DIO_PIN13,LOW);
					DIO_WritePin(DIO_PIN14,LOW);
					DIO_WritePin(DIO_PIN15,HIGH);
					_delay_ms(300);
					break;
				}

			}
		}

		CPU_SleepEnable();
	}
}
