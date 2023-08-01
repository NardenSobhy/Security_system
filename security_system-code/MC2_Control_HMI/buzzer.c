 /******************************************************************************
 *
 * Module: BUZZER
 *
 * File Name: buzzer.c
 *
 * Description: Source file for the Buzzer driver
 *
 * Author: Narden Sobhy Shaker
 *
 *******************************************************************************/

#include "gpio.h"
#include "common_macros.h"
#include "buzzer.h"

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/

/*Description
 *The Function responsible for setup the direction for the buzzer pin.
 *turn off the buzzer at the beginning
 */
void Buzzer_init() {
	/*Make pin of buzzer output pins*/
	GPIO_setupPinDirection(BUZZER_PORT, BUZZER_PIN, PIN_OUTPUT);
	/*turn off the buzzer at the beginning*/
	GPIO_writePin(BUZZER_PORT, BUZZER_PIN, LOGIC_LOW);
}

/*
 * Description :
 * Function to enable the Buzzer through the GPIO.
 */
void Buzzer_on(){
	GPIO_writePin(BUZZER_PORT, BUZZER_PIN, LOGIC_HIGH);
}

/*
 * Description :
 * Function to disable the Buzzer through the GPIO.
 */
void Buzzer_off(){
	GPIO_writePin(BUZZER_PORT, BUZZER_PIN, LOGIC_LOW);
}
