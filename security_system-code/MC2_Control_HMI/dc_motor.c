/******************************************************************************
 *
 * Module: DC Motor
 *
 * File Name: dc_motor.c
 *
 * Description: Source file for the DC Motor driver
 *
 * Author: Narden Sobhy Shaker
 *
 *******************************************************************************/

#include "dc_motor.h"
#include "gpio.h"
#include "pwm.h"

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/

void DcMotor_Init(void){



	/*Make two pin of motors output pins*/
	GPIO_setupPinDirection(DCMOTOR_OUT1_PORT, DCMOTOR_OUT1_PIN, PIN_OUTPUT);
	GPIO_setupPinDirection(DCMOTOR_OUT2_PORT, DCMOTOR_OUT2_PIN, PIN_OUTPUT);
	/*STOP motor at the beginning*/
	GPIO_writePin(DCMOTOR_OUT1_PORT, DCMOTOR_OUT1_PIN, LOGIC_LOW);
	GPIO_writePin(DCMOTOR_OUT2_PORT, DCMOTOR_OUT2_PIN, LOGIC_LOW);

}

void DcMotor_Rotate(DcMotor_State state,uint8 speed){


	Timer0_PWM_Init(speed);
	switch (state) {
	case STOP:
		GPIO_writePin(DCMOTOR_OUT1_PORT, DCMOTOR_OUT1_PIN, LOGIC_LOW);
		GPIO_writePin(DCMOTOR_OUT2_PORT, DCMOTOR_OUT2_PIN, LOGIC_LOW);
		break;
	case CW:
		GPIO_writePin(DCMOTOR_OUT1_PORT, DCMOTOR_OUT1_PIN, LOGIC_LOW);
		GPIO_writePin(DCMOTOR_OUT2_PORT, DCMOTOR_OUT2_PIN, LOGIC_HIGH);
		break;
	case A_CW:
		GPIO_writePin(DCMOTOR_OUT1_PORT, DCMOTOR_OUT1_PIN, LOGIC_HIGH);
		GPIO_writePin(DCMOTOR_OUT2_PORT, DCMOTOR_OUT2_PIN, LOGIC_LOW);
		break;
	}
}
