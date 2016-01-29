/*
 * OOPL Configuration Menu
 *
 *  Created by James Moran on 8/31/09, james@outofplanelabs.com
 *  Copyright 2009 Out-of-Plane Labs, LLC. All rights reserved.
 *
 */

#include <htc.h>
#include <string.h>

#include "config_menu.h"

#define EXIT 	  0
#define CONTINUE  1

void Send_Char(char c)
{
	// wait for clearance...
  	while(!TXIF)
		;
	TXREG = c;
}


void Send_String(char *string)
{
	char cnt;

	for(cnt = 0; cnt < strlen(string); cnt++)
  	{
    	Send_Char(string[cnt]);
	}
}


int Start_Config_Menu(char *command)
{
	Send_String((char *)"OK\r\n");

	if(command[0] == 'a' && command[1] == 't')
	{
		// UART Baud rate
		if(command[2] == 'b' && command[3] == 'd')
		{
			//Send_String((char *)"bd\r\n");
			//if(command[5] >= 0x00 && command[5] <= 0x06)
			if(strlen(command) > 5)
			{
				Send_String((char *)"Set baud rate\r\n");
				// set requested baud rate
				return CONTINUE;
			}
			else
			{
				Send_String((char *)"Return current baud rate\r\n");
				// return current baud setting
				return CONTINUE;
			}

		}
		
		// I2C Address
		else if(command[2] == 'i' && command[3] == 'd')
		{
			//Send_String((char *)"id\r\n");
			
			if(strlen(command) > 5)
			{
				Send_String((char *)"Set I2C address\r\n");
				// set requested I2C address
				return CONTINUE;
			}
			else
			{
				Send_String((char *)"Return current I2C address\r\n");
				// return current I2C address
				return CONTINUE;
			}			

			//return CONTINUE;
		}
	
		// Look-up Table
		else if(command[2] == 'l' && command[3] == 't')
		{
			//Send_String((char *)"lt\r\n");

			if(strlen(command) > 5)
			{
				Send_String((char *)"Set look-up table\r\n");
				// set requested look-up table
				return CONTINUE;
			}
			else
			{
				Send_String((char *)"Return current look-up table\r\n");
				// return current look-up table
				return CONTINUE;
			}
			
			//return CONTINUE;
		}
		
		// Firmware Version
		else if(command[2] == 'v' && command[3] == 'r')
		{
			Send_String((char *)"V. ");
			//Send_String((char *)eeprom_read(0x00));
			//Send_Char(eeprom_read(FIRMWARE_VERSION));
			Send_String((char *)"\r\n");
			return CONTINUE;
		}

		// Guard Mode Time-out
		else if(command[2] == 'g' && command[3] == 't')
		{
			//Send_String((char *)"gt\r\n");
		
			if(strlen(command) > 5)
			{
				Send_String((char *)"Set guard mode time-out\r\n");
				// set requested time-out
				return CONTINUE;
			}
			else
			{
				Send_String((char *)"Return guard mode time-out\r\n");
				// return current time-out
				return CONTINUE;
			}

			//return CONTINUE;
		}

		// Exit Config Mode
		else if(command[2] == 'c' && command[3] == 'n')
		{
			Send_String((char *)"Exit\r\n");
			return EXIT;
		}

		else
		{
			Send_String((char *)"Invalid command 1\r\n");
			return CONTINUE;
		}	
	}
	else
	{
		Send_String((char *)"Invalid command 2\r\n");
		return CONTINUE;
	} 	
}
