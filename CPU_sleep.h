/******************************************************************************
 *
 * Module: CPU sleeping mood
 *
 * File Name: CPU_sleep.h
 *
 * Description: Header file for CPU sleep Driver
 *
 * Created on: Sep 13, 2019
 *
 * Author: Ahmed Eldakhly & Hesham Hafez
 *
 *******************************************************************************/

#ifndef CPU_SLEEP_H_
#define CPU_SLEEP_H_

/*******************************************************************************
 *                       	Included Libraries                                 *
 *******************************************************************************/
#include "std_types.h"

/*******************************************************************************
 *                      Preprocessor Macros                                    *
 *******************************************************************************/
#define CPU_MCUCR (*(volatile uint8*)0x55)

#define CPU_SE		 7
#define CPU_SM2		 6
#define CPU_SM1		 5
#define CPU_SM0		 4

/*******************************************************************************
 *                      Functions Prototypes                                   *
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
extern void CPU_Sleep(void);

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
extern void CPU_SleepEnable(void);

/*******************************************************************************
 * Function Name:	CPU_SleepDisable
 *
 * Description: 	Enable Selected CPU Sleeping Mood
 *
 * Inputs:			NULL
 *
 * Outputs:			NULL
 *
 * Return:			NULL
 *******************************************************************************/
extern void CPU_SleepDisable(void);

#endif /* CPU_SLEEP_H_ */
