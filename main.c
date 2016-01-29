/*
 *  ServoController
 *  
 *
 *  Created by James Moran on 8/31/09, james@outofplanelabs.com
 *  Copyright 2009 Out-of-Plane Labs, LLC. All rights reserved.
 *
 */
 
#include <htc.h>
#include <string.h>

#include "config_menu.h"

__CONFIG(0x00F4); // 8mhz int
//__CONFIG(0x30D2); //hs external

// Using Internal Clock of 8 Mhz
#define FOSC 		 8000000L

// UART/I2C select jumper pin
#define I2C_JUMPER 	 RA4

// UART LED
#define UART_LED	 RA5

// I2C LED
#define I2C_LED 	 RA2


#define START_BYTE  	0x55  // ASCII "U", used for Automatic Baud Detection
#define VALID_ACK		0x42

// Servo serial commands
#define SET_POSITION		0x01
#define SERVO_OFF			0x0D
#define RESET_CONTROLLER	0x52


#define SERVO_OFFSET    0xFA5C  
#define SERVO_STEP	    1173

const unsigned short positionTable[256] = {
	64092 , 64081 , 64069 , 64057 , 64046 , 64034 , 64022 , 64010 , 
	63999 , 63987 , 63975 , 63963 , 63952 , 63940 , 63928 , 63917 , 
	63905 , 63893 , 63881 , 63870 , 63858 , 63846 , 63834 , 63823 , 
	63811 , 63799 , 63788 , 63776 , 63764 , 63752 , 63741 , 63729 , 
	63717 , 63705 , 63694 , 63682 , 63670 , 63658 , 63647 , 63635 , 
	63623 , 63612 , 63600 , 63588 , 63576 , 63565 , 63553 , 63541 , 
	63529 , 63518 , 63506 , 63494 , 63483 , 63471 , 63459 , 63447 , 
	63436 , 63424 , 63412 , 63400 , 63389 , 63377 , 63365 , 63354 , 
	63342 , 63330 , 63318 , 63307 , 63295 , 63283 , 63271 , 63260 , 
	63248 , 63236 , 63224 , 63213 , 63201 , 63189 , 63178 , 63166 , 
	63154 , 63142 , 63131 , 63119 , 63107 , 63095 , 63084 , 63072 , 
	63060 , 63049 , 63037 , 63025 , 63013 , 63002 , 62990 , 62978 , 
	62966 , 62955 , 62943 , 62931 , 62919 , 62908 , 62896 , 62884 , 
	62873 , 62861 , 62849 , 62837 , 62826 , 62814 , 62802 , 62790 , 
	62779 , 62767 , 62755 , 62744 , 62732 , 62720 , 62708 , 62697 , 
	62685 , 62673 , 62661 , 62650 , 62638 , 62626 , 62615 , 62603 , 
	62591 , 62579 , 62568 , 62556 , 62544 , 62532 , 62521 , 62509 , 
	62497 , 62485 , 62474 , 62462 , 62450 , 62439 , 62427 , 62415 , 
	62403 , 62392 , 62380 , 62368 , 62356 , 62345 , 62333 , 62321 , 
	62310 , 62298 , 62286 , 62274 , 62263 , 62251 , 62239 , 62227 , 
	62216 , 62204 , 62192 , 62181 , 62169 , 62157 , 62145 , 62134 , 
	62122 , 62110 , 62098 , 62087 , 62075 , 62063 , 62051 , 62040 , 
	62028 , 62016 , 62005 , 61993 , 61981 , 61969 , 61958 , 61946 , 
	61934 , 61922 , 61911 , 61899 , 61887 , 61876 , 61864 , 61852 , 
	61840 , 61829 , 61817 , 61805 , 61793 , 61782 , 61770 , 61758 , 
	61746 , 61735 , 61723 , 61711 , 61700 , 61688 , 61676 , 61664 , 
	61653 , 61641 , 61629 , 61617 , 61606 , 61594 , 61582 , 61571 , 
	61559 , 61547 , 61535 , 61524 , 61512 , 61500 , 61488 , 61477 , 
	61465 , 61453 , 61442 , 61430 , 61418 , 61406 , 61395 , 61383 , 
	61371 , 61359 , 61348 , 61336 , 61324 , 61312 , 61301 , 61289 , 
	61277 , 61266 , 61254 , 61242 , 61230 , 61219 , 61207 , 61195 , 
	61183 , 61172 , 61160 , 61148 , 61137 , 61125 , 61113 , 61101   
};


#define TIMER0_INIT     101  
#define TIMER1_INIT     4580 


#define TRUE  			1
#define FALSE 			0
#define ENABLED   		1 
#define DISABLED  		0 
#define SET       		1 
#define RESET       	0	 

// Timer 1 value
unsigned short T1; 

char baud_detected    = 1;
//char command_mode   = 0; 
char command_received = 0;

char servo;  
unsigned short pos[8];

void I2C_CommandParser(unsigned char);
void UART_CommandParser(unsigned char);
void Initialize();
void Serial_Initialize(unsigned char);


void interrupt interrupt_controller(void) 
{
    static unsigned short count = 0;
	unsigned char rx_byte; 

	if(RCIF)
	{
		count++;
    
        if(count % 10 == 0)
		{
			UART_LED = ~UART_LED;
		}

		/*if(!baud_detected)
		{
			SPBRG = SPBRG;
			rx_byte = RCREG;
			TXREG = 'D';
			baud_detected = TRUE;
			SPBRG--;
		}
		else
		{*/
			rx_byte = RCREG;
			UART_CommandParser(rx_byte);
		//}
	}

	if(SSPIF)
	{
		count++;

		if(count % 10 == 0)
		{
			I2C_LED = ~I2C_LED;
		}	
		
		if(SSPOV) // SSP Overflow
		{
			SSPOV = 0;
		}
	
		rx_byte = SSPBUF;

		if(DA == 1)//last byte was a data byte
		{
			I2C_CommandParser(rx_byte);
		}

		SSPIF = 0;
	}

	if(T0IF)	            			 
	{ 
		TMR0 = TIMER0_INIT;                 //load Frame 196 x 12.8us = 2508us interrupt cycle 
		
		PORTC = PORTC & ~(1 << servo);      // force servo off...in case value was wrong 
		 
		servo = (servo + 1) % 8;            // next servo

		PORTC = PORTC | (1 << servo);       // turn on next servo 
									            
		TMR1H = pos[servo] >> 8;    	    // load servo position...each count is 0.5us   0xFA5C gives 748us 
		TMR1L = pos[servo] & 0x00FF;	    // To set time: TMR1 = 65535 - (time / .5us)   0xEEAD gives 2.24

		TMR1ON = ENABLED;                   // turn Timer1 On 
		TMR1IF = RESET;                     // reset Timer1 Interrupt Flag 
		T0IF = RESET;                       // reset Timer0 Interrupt Flag
	}									 
	else if(TMR1IF)                    
	{                                 
		PORTC = PORTC & ~(1 << servo);      // turn off servo
		TMR1ON = DISABLED;                  // turn Timer1 Off 
	}
}


// I2C Command format is:
//    [address] [command] [arg 1] [arg 2]...[arg n]
// Valid commands are:
//    0x01 - Set Servo
//         arg1 = servo number, 0-7
//         arg2 = servo value,  0-255
//    0x0D - Turn Off Servo
//         arg1 = servo number, 0-7
//    0x52 - Reset Controller
void I2C_CommandParser(unsigned char byte)
{
	static char current_command;
	static char next_arg;

	static char current_servo;

	if(!command_received)
	{
		current_command = byte;  
		command_received = SET;
	}
	else if(command_received)
	{
		if(current_command == SET_POSITION)
		{
			if(next_arg == 0)
			{
				current_servo = byte;
				TRISC = TRISC & ~(1 << current_servo);
				next_arg++;
			}
			else if(next_arg == 1)
			{
				pos[current_servo] = positionTable[byte];
				next_arg = 0;
				//accept_serial = RESET;
				//command_mode = RESET;
				command_received = RESET;
				current_command = RESET;
				current_servo = 0;
			}
		}
		else if(current_command == SERVO_OFF)
		{
			if(next_arg == 0)
			{
				current_servo = byte;
				TRISC = TRISC | (1 << current_servo);
				//accept_serial = RESET;
				//command_mode = RESET;
				command_received = RESET;
				current_command = RESET;
			}
		}
		else if(current_command == RESET_CONTROLLER)
		{
			// TODO: FILL IN RESET COMMAND
		}
		else // Resets command parser if stuck in invalid state
		{
			//accept_serial = RESET;
			command_received = RESET;
			current_command = RESET;
			next_arg = RESET;
		}					
	}
}

// UART Command format is:
//    0x55 [command] [arg 1] [arg 2]...[arg n]
// Valid commands are:
//    0x01 - Set Servo
//         arg1 = servo number, 0-7
//         arg2 = servo value,  0-255
//    0x0D - Turn Off Servo
//         arg1 = servo number, 0-7
//    0x52 - Reset Controller
void UART_CommandParser(unsigned char byte)
{
	static char accept_serial = 0;
	static char current_command;
	static char next_arg;

	//static char menu_start = 0;
	//static char accept_config = 0;	
	//static char config_string[7]; 
	//static int config_length = 0;

	static char current_servo;

	///// START CONFIG MENU /////
	/*if(byte == '+')
	{
		menu_start++;

		if(menu_start == 3)
		{
			accept_config = 1;
			Send_String((char *)"OK\r\n");
			return;
		}
	}
	else
	{
		menu_start = 0;
	}

	if(accept_config)
	{
		TRISC = 0xFF;  // Disable servo outputs for safety
		
		if(byte != 0x0D && config_length <= 6) // end sentinel
		{	
			//TXREG = 'I';
			config_string[config_length] = byte;
			config_length++;							    
			//strcat(&config_string, &byte); // NOT WORKING PROPERLY
										   // String gets mangled
			return;
		}
		else if(config_length >= 3)
		{
			//TXREG = 'S';
			//config_string[config_length] = byte;
			config_string[config_length] = 0xFF;
			//strcat(&config_string, (char)0xFF);
			accept_config = Start_Config_Menu(&config_string);
			
			for(config_length = 0; config_length < 7; config_length++)
			{
				config_string[config_length] = 0x00;
			}
			config_length = 0;
			
		}
	}*/
    ///// END CONFIG MENU /////	
	
	/*else*/ if(!accept_serial && byte == START_BYTE)
	{
		accept_serial = SET;
	}
	else if(accept_serial)
	{
		if(!command_received)
		{
			current_command = byte;  
			command_received = SET;
		}
		else if(command_received)
		{
			if(current_command == SET_POSITION)
			{
				if(next_arg == 0)
				{
					current_servo = byte;
					TRISC = TRISC & ~(1 << current_servo);
					next_arg++;
				}
				else if(next_arg == 1)
				{
					pos[current_servo] = positionTable[byte];
					next_arg 		   = RESET;
					accept_serial      = RESET;
					//command_mode     = RESET;
					command_received   = RESET;
					current_command    = RESET;
					current_servo      = RESET;

					Send_Char(VALID_ACK);
				}
			}
			else if(current_command == SERVO_OFF)
			{
				if(next_arg == 0)
				{
					current_servo    = byte;
					TRISC            = TRISC | (1 << current_servo);
					accept_serial    = RESET;
					//command_mode   = RESET;
					command_received = RESET;
					current_command  = RESET;
					
					Send_Char(VALID_ACK);
				}
			}
			else if(current_command == RESET_CONTROLLER)
			{
				// TODO: FILL IN RESET COMMAND
				next_arg 		   = RESET;
				accept_serial      = RESET;
				//command_mode     = RESET;
				command_received   = RESET;
				current_command    = RESET;
				current_servo      = RESET;
			}					
		} 
	}
}


void Initialize()
{
	OSCCON = 0b01110001; 	// Clock config to internal 8mhz 
	while(HTS == 0);    	//Wait for frequency to stabilize
			
	TRISC  = 0xFF; //0x00;    
	ANSEL  = 0x00;        
	ANSELH = 0x00;       		
	OPTION = 0x84;			// Timer0 prescaler = 32 
	PORTC  = 0; 
	T1CON  = 0;             // Timer1 Off; No prescaler 
	TRISA  = 0xDB;  //0xFF; // RA2 and 5 outputs for LEDs
	WPUA   = 0x00;


	for(servo = 0; servo < 8; servo++)
	{
		pos[servo] = positionTable[127];  //0xFFFF; // set for 1500us mid-point 
		//pos[servo] = 8000;
	}
	
	servo = 0; 
 
	TMR1H = TIMER1_INIT >> 8;			 // load Timer1 
    TMR1L = TIMER1_INIT & 0x00FF;

	TMR1IF = RESET;                  	 // reset Timer1 Interrupt Flag 
	TMR1IE = ENABLED;               	 // Timer1 Interrupt Enabled 
	TMR1ON = ENABLED;                    // Timer1 On 
	//TMR1ON = DISABLED;
	TMR0 = TIMER0_INIT;            	     // load TMR0 for 2500us Interrupt Cycle Count 
	T0IE = ENABLED;                      // reset Timer0 Interrupt Flag 

    PORTC = PORTC | (1 << servo);		 // turn on first servo 

	Serial_Initialize(I2C_JUMPER);

	PEIE = ENABLED;
	GIE = ENABLED;
}

void Serial_Initialize(unsigned char mode)
{
	// Clear everything first
	baud_detected = FALSE;

	TRISB4  = 0;
	TRISB6  = 0;
	SSPCON  = 0;
	SSPMSK  = 0;
	SSPCON  = 0;
	SSPADD  = 0;
    SSPSTAT = 0;
	SSPIE   = 0;
	SPBRGH  = 0;
	SPBRG   = 0;	
	BRGH    = 0;
	BRG16   = 0;	
    TXSTA   = 0; 
    RCSTA   = 0; 
	SCKP    = 0; 
	RCIE    = 0;

	// I2C Slave
	if(mode)  // I2C_Jumper -> Configure I2C
	{
		TRISB4 = 1;
		TRISB6 = 1;
	
		//Workaround as Specified by Errata DS80243F
		SSPCON = 0b00111001;
    	//xxxx 0110 = I2C Slave mode, 7 bit address, no interrupts on start and stop
    	//xxx1 xxxx = CKP = 1, Enable clock
    	//xx1x xxxx = SSPEN, Enable SSP
		//Configure as I2C slave at given address 
		SSPMSK = 0xFF;
		SSPCON = 0b00110110;
		SSPADD = I2C_ADDRESS;
	
	
		//All bits must be cleared
    	SSPSTAT = 0;
	
		SSPIE = 1;
		
	} // END I2C Slave
	else // !I2C_Jumper -> Configure UART
	{
		// Clear auto baud rate registers
		SPBRGH = 0;
		SPBRG  = 0;
		
		// Setup the hardware UART module and interrupts for 9600
   		SPBRG = 51;
		BRG16 = 0;

		// Setup the hardware UART module and interrupts for 115200
    	//SPBRG = 16;
		//BRGH  = 1;
		//BRG16 = 1;	

    	TXSTA = 0b00100100; //Enable Transmit BRGH to high speed
    	RCSTA = 0b10010000; //Receive enable, 8-bit asych continous receive mode
		SCKP = 0; // Transmit Inverted data
	
		RCIE = ENABLED;           // Receive interrupt enabled
		//TXIE = DISABLED; 			// Transmit interrupt disableds
	}   // END UART

	I2C_LED	 = mode;
	UART_LED = !mode;
}

int main()
{
	unsigned char rx_byte;
	unsigned char serial_state = I2C_JUMPER; 

    GIE = DISABLED;
	Initialize();

	
	/*if(!baud_detected && !I2C_JUMPER)
	{
		ABDEN = 1;
	}*/

	while(1) 
	{		

		//I2C_LED	 = serial_state;
		//UART_LED = !serial_state;	
		
		if(serial_state != I2C_JUMPER)
		{
			serial_state = I2C_JUMPER;
			Serial_Initialize(serial_state);
		}
	}
}