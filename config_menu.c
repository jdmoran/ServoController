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


void Start_Config_Menu(char *command)
{
	Send_String((char *)"OK\r\n");
	Send_String(command);
	
	if(command[0] == 'a' && command[1] == 't')
	{
		// UART Baud rate
		if(command[2] == 'b' && command[3] == 'd')
		{
			Send_String((char *)"bd\r\n");
			if(command[5] >= 0x00 && command[5] <= 0x06)
			{
				// set requested baud rate
			}
			else
			{
				// return current baud setting
			}

		}
		
		// I2C Address
		else if(command[2] == 'i' && command[3] == 'd')
		{
			Send_String((char *)"id\r\n");
		}
	
		// Look-up Table
		else if(command[2] == 'l' && command[3] == 't')
		{
			Send_String((char *)"lt\r\n");
		}
		
		// Firmware Version
		else if(command[2] == 'v' && command[3] == 'r')
		{
			Send_String((char *)"vr\r\n");
		}

		// Guard Mode Timeout
		else if(command[2] == 'g' && command[3] == 't')
		{
			Send_String((char *)"gt\r\n");
		}

		// Exit Config Mode
		else if(command[2] == 'c' && command[3] == 'n')
		{
			Send_String((char *)"cn\r\n");
		}

		else
		{
			Send_String((char *)"Invalid command 1\r\n");
		}	
	}
	else
	{
		Send_String((char *)"Invalid command 2\r\n");
	} 	
}