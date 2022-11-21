/*
 * oled.c
 *
 *  Created on: 13 lis 2022
 *      Author: miqix
 */
#include <avr/io.h>
#include "oled.h"
#include "oled_defs.h"
#include "I2C/TWI.h"


//
//Init commands table
//
uint8_t InitCommands[] = {
	0xAE,
	//0x20, 0b00,
	//0xB0,
	0xC8,		//common pins scan direction (page (y) addressing starting from bottom or from top)
	0x00,
	0x10,
	//0x40,
	0x81,0x3F,
	0xA1,		//segment re-map (segment (x) addressing starting from bottom or from top)
	//0xA6,
	0xA8, 63,	//multiplex ratio (display height)
	//0xA4,
	0xD3,0x01,	//offset, has to be 0x01 for my display
	//0xD5,
	//0xF0,
	//0xD9,0x22,
	//0xDA,0x12,
	0xDB,0x20,
	//0x8D,0x14, //dc - dc??
	0xAD,0x8B,		//dc-dc 8A - disabled 8B - enabled
	0xAF
};




//
//Basic Communication functions
//

void OLED_SendCmd(uint8_t command)
{
	I2C_start();
	I2C_write(OLED_ADDR);
	I2C_write(SEND_COMMAND); //control byte
	I2C_write(command);
	I2C_stop();
}

void OLED_Send2ByteCmd(uint8_t command, uint8_t value)
{
	I2C_start();
	I2C_write(OLED_ADDR);
	I2C_write(SEND_COMMAND); //control byte
	I2C_write(command);
	I2C_write(value);
	I2C_stop();
}

void OLED_SendData(uint8_t* Data, uint8_t length)
{
	uint8_t i;
	I2C_start();
	I2C_write(OLED_ADDR);
	I2C_write(0x40); //control byte

	for(i = 0; i < length; i++)
	{
		I2C_write(*(Data++));
	}
	I2C_stop();

}
void OLED_Init()
{
	uint8_t i;
	uint8_t CommandsSize = sizeof(InitCommands);
	I2C_start();
	I2C_write(OLED_ADDR);
	I2C_write(SEND_COMMAND);
	for(i = 0; i < CommandsSize ; i++)
	{
		I2C_write(InitCommands[i]);
	}
	I2C_stop();
}


void OLED_DrawPixel(uint8_t x, uint8_t y, uint8_t color)
{
	x += COLLUMN_CENTER_OFFSET;
	if(x > 131 - COLLUMN_CENTER_OFFSET || x < COLLUMN_CENTER_OFFSET || y > DISPLAY_HEIGHT)
	{
		return;
	}
	uint8_t PixelNumber = ( color << (y % 8) );	//Pixel number in 1 byte organized VRAM memory

	OLED_SendCmd( SET_PAGE | (y/8) );	//Set page address, each page has 8 pixels
	OLED_SendCmd( x & 0x0F ); 			//Set lower column address (4 lower bits 0b0000xxxx)
	OLED_SendCmd( (x >> 4) | 0x10 ); 	//Set higher column address	(4 higher bits + 0x10 identifier 0b0001xxxx)
	OLED_SendData(&PixelNumber,1);
}
