/*
 ================================================================================================
 Name        : HMI_ECU.h
 Author      : Narden Sobhy Shaker
 Description : This microcontroller is just responsible for interaction with the user just take inputs through keypad and display messages on the LCD.
 Date        : 26/10/2022
 ================================================================================================
*/

#ifndef HMI_ECU_H_
#define HMI_ECU_H_

/*******************************************************************************
 *                                Include                                      *
 *******************************************************************************/
#include "lcd.h"
#include "keypad.h"
#include "uart.h"
#include "timer1.h"
#include <util/delay.h>
#include <avr/io.h>

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/
#define PASSWORD_SIZE     5
#define ERROR             0
#define SUCCESS           1
#define MC1_READY         0x10
#define DOOR_OPEN_SECONDS 33
#define ENTER_KEY		  13    /* 13 is the "ON/C" button on the keypad */



typedef enum{
	MAIN_LIST,ENTER_PASS,CREATE_PASS,OPEN_DOOR,FAIL
}states;
/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/*
 * Description: Function to display door is open for 33 seconds
 */
void Open_Door(void);



/*
 * Description: Function to display warning message for 1 mins
 */
void alarm();

/*
 * Description: Function to enter new password for system
 * enter new password
 * re enter new password
 * if match send to control ECU to save it in eeprom
 */

void create_Password();

/*
 * Description: Function to enter password to open door or change password
 * enter password
 * send to control ECU to match it with system password
 */

void user_Password();

/*
 * Description : Function to send password to Control ECU
 */

void Send_Password(uint8 *password );

/*
 * Description: Function to check if first password of system is created.
 */
void check_First_Password();

#endif /* HMI_ECU_H_ */
