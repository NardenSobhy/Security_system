/*
 ================================================================================================
 Name        : Control_ECU.h
 Author      : Narden Sobhy Shaker
 Description : This microcontroller is just responsible is responsible for all the processing and decisions in the system like password checking, open the door and activate the system alarm.
 Date        : 26/10/2022
 ================================================================================================
*/


#ifndef CONTROL_ECU_H_
#define CONTROL_ECU_H_

/*******************************************************************************
 *                                Include                                      *
 *******************************************************************************/

#include "uart.h"
#include "twi.h"
#include "external_eeprom.h"
#include "timer1.h"
#include "buzzer.h"
#include "dc_motor.h"
#include <util/delay.h>
#include <avr/io.h>

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/


#define PASSWORD_SIZE       5
#define MC2_READY          0x10
#define OPEN_DOOR_SECOND   15
#define HOLD_DOOR_SECOND    18
#define CLOSE_DOOR_SECOND   33
#define DEFUALTVALUE       0XFF
#define PASSWORD_ADDRESS  0x0315

typedef enum{
	MAIN_LIST,ENTER_PASS,CREATE_PASS,OPEN_DOOR,FAIL
}states;

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/


/*
 *  * Description: Function to create new password  and save it in eeprom
 */
void Create_Password();



/*
 * Description: Function to open the door
 * 15 seconds CW (OPEN)
 * 3 seconds hold (HOLD)
 * 15 seconds ACW (CLOSE)
 */

void Open_Door(void);


/*
 * Description: Function to turn on buzzer for 1 minute
 */

void alarm();


/*
 * Description: Function to check if it is fist using of the system
 * check if there are password in saved at memory or not
 * send flag to HMI_ECU
 *
 */

void check_First_Passowrd();


/*
 * Description : Function to recieve password from HMI-ECU
 */

void recieve_Password(uint8 *password);


/*
 * Description: Function to recieve password form HMI-ECU to open door or change password
 * Recieve password
 * match it with system password and return success or error
 */

uint8 Compare_Password();


#endif /* CONTROL_ECU_H_ */
