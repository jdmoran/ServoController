/*
 * OOPL Configuration Menu
 *
 *  Created by James Moran on 8/31/09, james@outofplanelabs.com
 *  Copyright 2009 Out-of-Plane Labs, LLC. All rights reserved.
 *
 * EEPROM Map
 * 0x00 - Firmware version
 * 0x01 - UART auto baud-detect on/off
 * 0x02 - UART baud-rate (SPBRGH)
 * 0x03 - UART baud-rate (SPBRG)
 * 0x04 - I2C address
 *
 */
#ifndef _CONFIG_MENU_H_
#define _CONFIG_MENU_H_

// EEPROM Address Map
#define FIRMWARE_VERSION  0x00
#define AUTO_BAUD_DETECT  0x01
#define BAUD_RATE_HIGH	  0x02
#define BAUD_RATE_LOW	  0x03
#define I2C_ADDRESS	      0x04


void Send_Char(char);
void Send_String(char *);
int Start_Config_Menu(char *);


#endif