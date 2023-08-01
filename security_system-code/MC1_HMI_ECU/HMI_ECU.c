/*
 ================================================================================================
 Name        : HMI_ECU.c
 Author      : Narden Sobhy Shaker
 Description : This microcontroller is just responsible for interaction with the user just take inputs through keypad and display messages on the LCD.
 Date        : 26/10/2022
 ================================================================================================
*/

#include "HMI_ECU.h"

/*******************************************************************************
 *                           Global Variables                                  *
 *******************************************************************************/

uint8 password[PASSWORD_SIZE] = { 0 };
uint16 ticks = 0;

/* Initialize the Timer Driver by its configrations */
Timer1_ConfigType TIMER1_Config = {0,8000,F_CPU_1024,COMPARE};
/* Initialize the UART Driver by its configrations */
UART_ConfigType UART_Config={EIGHT_BITS,ENABLED_ODD,ONE_BIT,BR_9600};


/*******************************************************************************
 *                       Interrupt Service Routines                            *
 *******************************************************************************/

/*
 * Description: call back Function by timer to calculate time of door open
 * 15 seconds CW
 * 3 seconds
 * 15 seconds ACW
 */

void TIMER1_MOTOR_ISR(void) {
	/*ticks to calculate time*/
	ticks++;
}

/*
 * Description: call back Function by timer to calculate time of alarm
 */

void TIMER1_ALARM_ISR() {
	ticks++;
}

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/


/*
 * Description : Function to send password to Control ECU
 */

void Send_Password(uint8 *password ){
	uint8 i;
	while (UART_recieveByte() != MC1_READY) {}
	for (i = 0; i < PASSWORD_SIZE; i++) {
		UART_sendByte(password[i]);
	}

}



/*
 * Description: Function to check if first passowrd of system is created.
 */

void check_First_Password(){
	uint8 flag = 0; /*flag to check if first pass of the system is created */
	UART_sendByte(MC1_READY);
	flag = UART_recieveByte();
	if (flag == 1) {
		create_Password();
	}
}



/*
 * Description: Function to display door is open for 33 seconds
 */
void Open_Door(void) {
	Timer1_setCallBack(&TIMER1_MOTOR_ISR);
	Timer1_init(&TIMER1_Config);
	while (1) {
		LCD_displayStringRowColumn(0, 0, "Door is open");
		if (ticks > DOOR_OPEN_SECONDS) {
			ticks = 0;
			Timer1_deInit();
			LCD_clearScreen();
			break;
		}
	}
}


/*
 * Description: Function to display warning message for 1 mins
 */
void alarm() {
	Timer1_setCallBack(&TIMER1_ALARM_ISR);
	Timer1_init(&TIMER1_Config);
	while (1) {
		LCD_displayStringRowColumn(0, 5, "ERROR ");
		if (ticks == 60) {
			ticks = 0;
			Timer1_deInit();
			LCD_clearScreen();
			break;
		}
	}
	LCD_clearScreen();
}

/*
 * Description: Function to enter new password for system
 * enter new password
 */

void create_Password() {
	uint8 password[PASSWORD_SIZE] ;
	uint8 Password_confirmation[PASSWORD_SIZE] ;
	uint8 key = 0;
	uint8 i=0;
	uint8 error=FALSE;
	while (1) {
		LCD_clearScreen();
		LCD_displayString("Plz Enter Pass:");
		LCD_moveCursor(1, 0);
		/*first loop to enter first password */
		while (1) {
			key = KEYPAD_getPressedKey();
			if (key >= 0 && key <= 9 ) {
				password[i] = key;
				i++;
				LCD_displayCharacter('*');
				_delay_ms(500);
				if(i==5){
					i=0;
					break;
				}
			}
		}
		while(KEYPAD_getPressedKey() != ENTER_KEY){} /* wait tell the user press Enter key on the keypad */
		LCD_clearScreen();
		LCD_displayStringRowColumn(0,0,"Plz Re-Enter the");
		LCD_displayStringRowColumn(1, 0, "same pass: ");
		/*second loop to enter second password */
		while (1) {
			key = KEYPAD_getPressedKey();
			if (key >= 0 && key <= 9 ) {
				Password_confirmation[i] = key;
				i++;
				LCD_displayCharacter('*');
				_delay_ms(500);
				if(i==5){
					i=0;
					break;
				}
			}
		}
		while(KEYPAD_getPressedKey() != ENTER_KEY){} /* wait tell the user press Enter key on the keypad */
		Send_Password(password);
		Send_Password(Password_confirmation);
		//UART_sendByte(READY);
		LCD_clearScreen();
		error=UART_recieveByte();
		if(error==FALSE){
			LCD_displayStringRowColumn(0, 0, "Correct");
			LCD_displayStringRowColumn(1, 0, "Password created");
			_delay_ms(500);
			break;
		}else{
			LCD_displayStringRowColumn(0, 0, "Wrong pass");
			_delay_ms(500);
			continue;
		}
	}
}

/*
 * Description: Function to enter password to open door or change password
 * enter password
 * send to control ECU to match it with system password
 */
void user_Password() {
	uint8 key = 0;
	uint8 i = 0;
	uint8 enter_password[PASSWORD_SIZE] = { 0 };
	LCD_clearScreen();
	LCD_displayString("Enter Password:");
	LCD_moveCursor(1, 0);
	/*loop to enter password */
	while (1) {
		key = KEYPAD_getPressedKey(); /*take value from keypad , if it is number save in password*/
		enter_password[i] = key;
		LCD_displayCharacter('*');
		i++;
		_delay_ms(500);
		/*break loop when enter five numbers of password*/
		if(i==5){
			LCD_clearScreen();
			i=0;
			break;
		}
	}
	Send_Password(enter_password);
}

/*******************************************************************************
 *                             Main Function                                   *
 *******************************************************************************/

int main(void) {
	SREG |= (1 << 7); /*Enable global interrupt*/
	uint8 key = 0;
	uint8 check_match = SUCCESS;
	uint8 allowed_tries = 0;
	states state = CREATE_PASS;
	/* Initialize the LCD Driver */
	LCD_init();
	/*read from eeprom the size of password to know if it first time use or not*/
	UART_init(&UART_Config);
	check_First_Password();
	while (1) {
		switch (state){
		case MAIN_LIST:
			LCD_displayStringRowColumn(0, 0, "+: Open door");
			LCD_displayStringRowColumn(1, 0, "-: Change Pass  ");
			key = KEYPAD_getPressedKey();
			_delay_ms(500);
			UART_sendByte(key);
			if (key == '-' || (key == '+')) {
				state =ENTER_PASS;
			}
			break;
		case ENTER_PASS:
			do {
				/*in two case user must enter the password*/
				user_Password();
				check_match = UART_recieveByte();
				allowed_tries++;
				/*if enter password three times wrong system will display warning message for 1 minute*/
				if (allowed_tries == 3) {
					state=FAIL;
				}
			} while (check_match == ERROR && allowed_tries != 3);
			allowed_tries = 0;
			if(check_match == SUCCESS){
				if (key == '-') {
					state=CREATE_PASS;
				}else if(key== '+'){
					state=OPEN_DOOR;
				}else{
					state=MAIN_LIST;
				}
			}
			break;
		case CREATE_PASS:
			create_Password();
			state=MAIN_LIST;
			break;
		case OPEN_DOOR:
			LCD_clearScreen();
			Open_Door();
			state=MAIN_LIST;
			break;
		case FAIL:
			LCD_clearScreen();
			alarm();
			state=MAIN_LIST;
			break;
		}
	}
}
