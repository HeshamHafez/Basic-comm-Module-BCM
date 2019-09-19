/******************************************************************************
 *
 * Module: CPU sleeping mood
 *
 * File Name: CPU_sleep.c
 *
 * Description: Source file for CPU sleep Driver
 *
 * Created on: Sep 13, 2019
 *
 * Author: Ahmed Eldakhly & Hesham Hafez
 *
 *******************************************************************************/

/*******************************************************************************
 *                       	Included Libraries                                 *
 *******************************************************************************/
#include "CPU_sleep.h"
#include "std_types.h"
#include "Macro.h"

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/

/*******************************************************************************
 * Function Name:	CPU_Sleep
 *
 * Description: 	Select CPU Sleeping Mood
 *
 * Inputs:			NULL
 *
 * Outputs:			NULL
 *
 * Return:			NULL
 *******************************************************************************/
void CPU_Sleep(void)
{
	/*select idle mode*/
	CLEAR_BIT(CPU_MCUCR,CPU_SM0);
	CLEAR_BIT(CPU_MCUCR,CPU_SM1);
	CLEAR_BIT(CPU_MCUCR,CPU_SM2);
}

/*******************************************************************************
 * Function Name:	CPU_SleepEnable
 *
 * Description: 	Enable Selected CPU Sleeping Mood
 *
 * Inputs:			NULL
 *
 * Outputs:			NULL
 *
 * Return:			NULL
 *******************************************************************************/
void CPU_SleepEnable(void)
{
	/*set sleep enable bit*/
	SET_BIT(CPU_MCUCR,CPU_SE);
}

/*******************************************************************************
 * Function Name:	CPU_SleepDisable
 *
 * Description: 	Disable Selected CPU Sleeping Mood
 *
 * Inputs:			NULL
 *
 * Outputs:			NULL
 *
 * Return:			NULL
 *******************************************************************************/
void CPU_SleepDisable(void)
{
	/*clear sleep enable bit*/
	CLEAR_BIT(CPU_MCUCR,CPU_SE);
}
