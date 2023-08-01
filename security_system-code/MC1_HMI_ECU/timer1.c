 /******************************************************************************
 *
 * Module: TIMER
 *
 * File Name: timer1.c
 *
 * Description: Source file for the ATmega16 Timer driver
 *
 * Author: Narden Sobhy Shaker
 *
 *******************************************************************************/

#include "timer1.h"
#include "common_macros.h"
#include <avr/io.h>
#include <avr/interrupt.h> /* For timer ISR */

/*******************************************************************************
 *                           Global Variables                                  *
 *******************************************************************************/

/* Global variables to hold the address of the call back function in the application */
static volatile void (*g_callBackPtr)(void) = NULL_PTR;

/*******************************************************************************
 *                       Interrupt Service Routines                            *
 *******************************************************************************/

ISR(TIMER1_COMPA_vect){

	if(g_callBackPtr != NULL_PTR)
	{
		/* Call the Call Back function in the application after the edge is detected */
		(*g_callBackPtr)(); /* another method to call the function using pointer to function g_callBackPtr(); */
	}
}
ISR(TIMER1_OVF_vect){

	if(g_callBackPtr != NULL_PTR)
	{
		/* Call the Call Back function in the application after the edge is detected */
		(*g_callBackPtr)(); /* another method to call the function using pointer to function g_callBackPtr(); */
	}
}
/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/

/*
* Description : Function to initialize the Timer driver
*/
void Timer1_init(const Timer1_ConfigType * Config_Ptr){
	TCNT1 = Config_Ptr->initial_value;   /* Set timer1 initial value */


	/* Configure timer control register TCCR1A
	    * 1. Disconnect OC1A and OC1B  COM1A1=0 COM1A0=0 COM1B0=0 COM1B1=0
		* 2. FOC1A=1 FOC1B=1
	 */
	TCCR1A = (1<<FOC1A)|(1<<FOC1B);

	/*Set Timer Mode*/
	if (Config_Ptr->mode == COMPARE) {
		 /*for compare match mode WGM10=0,WGM11=0,WGM12=1,WGM13=0 */
		TCCR1B = (1 << WGM12);
		/*insert the required compare match value in OCR1A Register*/
		OCR1A = Config_Ptr->compare_value;
		/* Enable the compare match interrupt to generate an interrupt when compare match occurs*/
		SET_BIT(TIMSK, OCIE1A);
	} else if (Config_Ptr->mode == NORMAL) {
		/* Enable the overflow interrupt to generate an interrupt when overflow occurs*/
		SET_BIT(TIMSK, TOIE1);
	}

	/* insert the required clock value in the first three bits (CS10, CS11 and CS12) of TCCR1B Register*/
	TCCR1B = (TCCR1B & 0XF8) | (Config_Ptr->prescaler & 0x07);

}

/*
 * Description: Function to disable the Timer1.
 */
void Timer1_deInit(void){

	/* Clear All Timer1 Registers */
	TCCR1A = 0;
	TCCR1B = 0;
	TCNT1 = 0;
	ICR1 = 0;
	/* Disable the interrupt */
	TIMSK &= ~(1 << TOIE1) & ~(1 << OCIE1A);

}
/*
 * Description: Function to set the Call Back function address.
 */
void Timer1_setCallBack(void(*a_ptr)(void)){

	/* Save the address of the Call back function in a global variable */
	g_callBackPtr = a_ptr;

}


