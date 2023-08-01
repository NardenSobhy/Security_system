/******************************************************************************
 *
 * Module: DC Motor
 *
 * File Name: dc_motor.h
 *
 * Description: Header file for the DC Motor driver
 *
 * Author: Narden Sobhy Shaker
 *
 *******************************************************************************/

#ifndef DC_MOTOR_H_
#define DC_MOTOR_H_

#include "std_types.h"
#include "avr/io.h"
/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/

#define DCMOTOR_OUT1_PORT PORTC_ID
#define DCMOTOR_OUT1_PIN  PIN2_ID

#define DCMOTOR_OUT2_PORT PORTC_ID
#define DCMOTOR_OUT2_PIN  PIN3_ID

/*******************************************************************************
 *                               Types Declaration                             *
 *******************************************************************************/

typedef enum
{
	STOP,CW,A_CW
}DcMotor_State;


/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

void DcMotor_Init(void);

void DcMotor_Rotate(DcMotor_State state,uint8 speed);

#endif /* DC_MOTOR_H_ */
