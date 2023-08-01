/*
 ================================================================================================
 Name        : Control_ECU.c
 Author      : Narden Sobhy Shaker
 Description : This microcontroller is just responsible is responsible for all the processing and decisions in the system like password checking, open the door and activate the system alarm.
 Date        : 26/10/2022
 ================================================================================================
*/


#include "Control-ECU.h"

/*******************************************************************************
 *                           Global Variables                                  *
 *******************************************************************************/

uint32 ticks = 0;
/*Timer Driver configrations */
Timer1_ConfigType TIMER1_Config = {0,8000,F_CPU_1024,COMPARE};

/*I2C Driver configrations */
TWI_ConfigType TWI_Config = {START_BYTE,FAST_MODE};

/*UART Driver configrations */
UART_ConfigType UART_Config={EIGHT_BITS,ENABLED_ODD,ONE_BIT,BR_9600};

/*******************************************************************************
 *                       Interrupt Service Routines                            *
 *******************************************************************************/


/*
 * Description: call back Function by timer to calcuate time of door open
 */

void TIMER1_MOTOR_ISR(void) {
	ticks++;
	/*disable timer and turn off motor after 33 seconds */
	if (ticks == CLOSE_DOOR_SECOND) {
		DcMotor_Rotate(STOP,0);
		ticks = 0;
		Timer1_deInit();
	}
}

/*
 * Description: call back Function by timer to calcuate time of Runing Buzzer
 */

void TIMER1_ALARM_ISR() {
	ticks++;
	/*disable timer and turn off buzzer after 1 minute*/
	if (ticks == 60) {
		ticks = 0;
		Buzzer_off();
		Timer1_deInit();
	}
}

/*******************************************************************************
 *                           Functions defination                              *
 *******************************************************************************/


/*
 * Description : Function to recieve password from HMI-ECU
 */

void recieve_Password(uint8 *password){
	uint8 i;
	UART_sendByte(MC2_READY);
	for (i = 0; i < PASSWORD_SIZE; i++) {
		password[i] = UART_recieveByte();
	}
}


/*
 * Description: Function to open the door
 * 15 seconds CW (OPEN)
 * 3 seconds hold (HOLD)
 * 15 seconds ACW (CLOSE)
 */
void Open_Door(void) {
	Timer1_setCallBack(&TIMER1_MOTOR_ISR);
	Timer1_init(&TIMER1_Config);
	while (ticks < CLOSE_DOOR_SECOND) {
		if (ticks < OPEN_DOOR_SECOND) {
			DcMotor_Rotate(CW,50);
		} else if (ticks < HOLD_DOOR_SECOND && ticks > OPEN_DOOR_SECOND) {
			DcMotor_Rotate(STOP,0);
		} else if (ticks < CLOSE_DOOR_SECOND && ticks > HOLD_DOOR_SECOND) {
			DcMotor_Rotate(A_CW,50);
		}
	}
}



/*
 * Description: Function to turn on buzzer for 1 minute
 */

void alarm() {
	Timer1_setCallBack(&TIMER1_ALARM_ISR);
	Timer1_init(&TIMER1_Config);
	ticks++;
	while (ticks < 60) {
		Buzzer_on();
	}
	Buzzer_off();
}

/*
 * Description: Function to check if it is fist using of the system
 * check if there are password in saved at memory or not
 * send flag to HMI_ECU
 *
 */

void check_First_Passowrd() {
	uint8 flag = 0;
	EEPROM_readByte(PASSWORD_ADDRESS, & flag);
	if (flag == DEFUALTVALUE) {
		flag = 1;
	}else{
		flag=0;
	}
	while (UART_recieveByte() != MC2_READY) {}
	UART_sendByte(flag);
	if (flag == 1) {
		Create_Password();
	}
}


/*
 *  * Description: Function to create new password  and save it in eeprom
 */

void Create_Password() {
	uint8 Password[PASSWORD_SIZE] ;
	uint8 Password_confirmation[PASSWORD_SIZE] ;
	uint8 i=0;
	uint8 error=FALSE;
	while(1){
		recieve_Password(Password);
		recieve_Password(Password_confirmation);
		for(i=0;i<PASSWORD_SIZE;i++){
			if(Password[i]!=Password_confirmation[i]){
				error=TRUE;
			}
		}
		UART_sendByte(error);
		if(error==TRUE){
			continue;
		}else{
			break;
		}
	}
	/*save new password in eeprom*/
	for (i = 0; i < PASSWORD_SIZE; i++) {
		EEPROM_writeByte(PASSWORD_ADDRESS + i, Password[i]); /* Write password in the external EEPROM */
		_delay_ms(10);
	}
}


/*
 * Description: Function to recieve password form HMI-ECU to open door or change password
 * Recieve password
 * match it with system password and return success or error
 */

uint8 Compare_Password() {
	uint8 i;
	uint8 password1[PASSWORD_SIZE] = { 0xff };
	uint8 password2[PASSWORD_SIZE] = { 0 };
	/*recieve enterd password from HMI_ECU*/
	recieve_Password(password1);
	/*Read system password which saved in eeprom*/
	for (i = 0; i < PASSWORD_SIZE; i++) {
		EEPROM_readByte(PASSWORD_ADDRESS + i, &password2[i]); /* Write password in the external EEPROM */
		_delay_ms(10);
	}
	/*compare between two password*/
	for (i = 0; i < PASSWORD_SIZE; i++) {
		if (password1[i] != password2[i]) {
			return ERROR;
		}
	}
	return SUCCESS;
}

/*******************************************************************************
 *                             Main Function                                   *
 *******************************************************************************/

int main() {
	uint8 key = 0;
	uint8 checkPassowrd = 0;
	uint8 allowed_tries = 0;
	states state=CREATE_PASS;
	/* Initialize the Motor Driver */
	DcMotor_Init();
	/* Initialize the buzzer Driver */
	Buzzer_init();
	/*enable global interrpt*/
	SREG |= (1 << 7);

	UART_init(&UART_Config);
	check_First_Passowrd();
	while (1) {
		switch (state){
		case MAIN_LIST:
			key = UART_recieveByte();
			if (key == '-' || key == '+') {
				state=ENTER_PASS;
			}
			break;
		case ENTER_PASS:
			do {
				checkPassowrd = Compare_Password();
				UART_sendByte(checkPassowrd);
				allowed_tries++;
				if (allowed_tries == 3) {
					state=FAIL;
				}
			} while (checkPassowrd == ERROR && allowed_tries != 3);
			allowed_tries = 0;
			if(checkPassowrd == SUCCESS){
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
			Create_Password();
			state=MAIN_LIST;
			break;
		case OPEN_DOOR:
			Open_Door();
			state=MAIN_LIST;
			break;
		case FAIL:
			alarm();
			state=MAIN_LIST;
			break;
		}
	}
}

