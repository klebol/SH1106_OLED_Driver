/*
 * oled.c
 *
 *  Created on: 13 lis 2022
 *      Author: miqix
 */
#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <avr/pgmspace.h>
#include "oled.h"
#include "oled_defs.h"
#include "oled_fonts.h"
#include "I2C/TWI.h"

#include "UART/uart.h"


//
//Global variables
//
static CursorPosition Cursor;
static FontInfo CurrentFont;
const uint8_t *ActualFont = StandardASCII;



//
//Init commands table
//
const uint8_t InitCommands[] PROGMEM = {
	DISPLAY_OFF,
	SET_COMM_SCAN_FBOT,
	SET_COLLUMN_ADDR_L,
	SET_COLLUMN_ADDR_H,
	SET_CONTRAST,0x3F,
	SET_SEGMENT_REMAP,
	SET_MULTIPLEX_RATIO, 63,
	SET_OFFSET,0,
	SET_VCOM,0x20,
	SET_DCDC,0x8B,
	DISPLAY_ON
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

void OLED_SendCmdSequence(uint8_t *commandSequence, uint8_t length)
{
	uint8_t i;
	I2C_start();
	I2C_write(OLED_ADDR);
	I2C_write(SEND_COMMAND); //control byte
	for(i = 0; i < length; i++)
	{
		I2C_write(*commandSequence);
		commandSequence++;
	}
	I2C_stop();
}

void OLED_SendData(uint8_t *data, uint8_t length)
{
	uint8_t i;
	I2C_start();
	I2C_write(OLED_ADDR);
	I2C_write(SEND_DATA); //control byte

	for(i = 0; i < length; i++)
	{
		I2C_write(*data);
		data++;
	}
	I2C_stop();

}

//
//End of Basic Communication functions
//


void OLED_Init()
{
	uint8_t i;
	uint8_t CommandSize = sizeof(InitCommands);
	uint8_t CommandBuffer[CommandSize];
	for(i = 0; i < CommandSize; i++)
	{
		CommandBuffer[i] = pgm_read_byte(InitCommands + i);	//InitCommands - table name as incremented pointer
	}
	OLED_SendCmdSequence(CommandBuffer,CommandSize);
	OLED_ChangeFont(StandardASCII);
}


void OLED_GoToCollumn(uint8_t CollumnNumber)
{
	CollumnNumber+=COLLUMN_CENTER_OFFSET;
	if(CollumnNumber > DISPLAY_WIDTH + COLLUMN_CENTER_OFFSET) return;
	OLED_SendCmd(SET_COLLUMN_ADDR_L | (CollumnNumber & 0x0F) ); 			//Set lower column address (4 lower bits 0b0000xxxx)
	OLED_SendCmd(SET_COLLUMN_ADDR_H | (CollumnNumber >> 4) ); 				//Set higher column address	(4 higher bits + 0x10 identifier 0b0001xxxx)
}

void OLED_MoveCursor(uint8_t collumn, uint8_t page)
{
	OLED_GoToCollumn(collumn);
	OLED_SendCmd(SET_PAGE | (page & 0x07) );		//0x07 mask to prevent numbers > 7
	Cursor.collumn = collumn;
	Cursor.page = page;
}

void OLED_GetCursorPosition(CursorPosition *OutputPosition)
{
	OutputPosition->collumn = Cursor.collumn;
	OutputPosition->page = Cursor.page;
}

void OLED_ClearDisp(void)
{
	uint8_t ZerosBuff[DISPLAY_WIDTH];
	uint8_t i;

	memset(&ZerosBuff, 0x00, sizeof(ZerosBuff));		//prepare zeros for clearing each page
	for(i = 0; i < DISPLAY_HEIGHT/8; i++)				//clear each page by sending zeros
	{
		OLED_MoveCursor(0,i);
		OLED_SendData(&ZerosBuff[0],sizeof(ZerosBuff));
	}
	OLED_MoveCursor(0,0);
}



//
//Functions for text writing - no buffer
//

void OLED_ChangeFont(const uint8_t *Font)
{
	CurrentFont.FontPointer = Font;
	CurrentFont.FontWidth = pgm_read_byte(Font + 1);
	CurrentFont.FontHeight = pgm_read_byte(Font + 2);
	CurrentFont.FirstChar = pgm_read_byte(Font + 3);
}

void OLED_WriteC(char character)
{
	switch(character)
	{
	case '\r':	//carriage return
		OLED_MoveCursor(0, Cursor.page);
		break;

	case '\n':	//linefeed, next line
		OLED_MoveCursor(Cursor.collumn, Cursor.page + 1);
		break;

	default: ;	//any other characters
		uint8_t i;
		uint8_t BytesToSend [MAX_FONT_WIDTH];
		uint16_t CharTableNumber = (character - CurrentFont.FirstChar ) * CurrentFont.FontWidth;  //calculate character position in font table
		const uint8_t *CharPointer = (CurrentFont.FontPointer + 4) + CharTableNumber;         //Increase pointer to this data

		for(i = 0; i < CurrentFont.FontWidth; i++)                                                //copy character from flash to buffer
		{
			BytesToSend[i] = pgm_read_byte(CharPointer);
			CharPointer++;
		}
		OLED_SendData(BytesToSend,CurrentFont.FontWidth);                                         //print it
		Cursor.collumn += CurrentFont.FontWidth;
		if(Cursor.collumn + CurrentFont.FontWidth > 127)
		{
			OLED_MoveCursor(0,Cursor.page + 1);
		}
		break;
	}
}

void OLED_WriteS(char *String)
{
	while(*String)
	{
		OLED_WriteC(*String);
		String++;
	}
}

void OLED_WriteI(int Value)
{
	char Buffer[5];
	sprintf(Buffer,"%d",Value);
	OLED_WriteS(Buffer);
}


uint8_t OLED_DrawBitmapFlash(uint8_t x, uint8_t y, const uint8_t *bitmap)
{
	uint8_t Width = pgm_read_byte(bitmap);               //Read bitmap width
	uint8_t Height = pgm_read_byte(bitmap + 1);          //Read bitmap height
	if(Width > DISPLAY_WIDTH || Height > DISPLAY_HEIGHT) return 1;
	if(x >= DISPLAY_WIDTH || y >= DISPLAY_HEIGHT) return 1;


	uint8_t BytesToDisplay[Width];
	uint8_t ShiftedFromPrevious[Width];
	uint8_t ShiftBuffer[Width];
	memset(&ShiftBuffer, 0x00, sizeof(ShiftBuffer));
	const uint8_t *ImageStart = bitmap + 2;

	uint8_t Page = y / 8;
	uint8_t BitShift = y % 8;					//value of bit shift
	uint8_t i,j;

	for(i = 0; i < (Height/8); i++)                              //Data is sent by pages to the bottom of the screen (no loop)
	{
		for(j = 0; j < Width; j++)                                      //Copying current page's image data to ram
		{
			BytesToDisplay[j] = pgm_read_byte(ImageStart + j);          //Read oryginal

			if(BitShift == 0) continue;                                 //skip part below if there is no need for shifting image
                                                                        /*Bit shifting part in case when y coordinate isnt page's 0 bit*/
			ShiftedFromPrevious[j] = ShiftBuffer[j];                    //Save bits which were shifted out from previus byte (page) in order to add them to this byte
			ShiftBuffer[j] = (BytesToDisplay[j] >> (8 - BitShift));	    //Save shifted out bits for next page
			BytesToDisplay[j] <<= BitShift;                             //Shift original page's byte
			BytesToDisplay[j] |= ShiftedFromPrevious[j];                //Add bits shifted out from prevous page's byte to current page's byte
		}
		OLED_MoveCursor(x, (Page + i) );                                //Move cursor to proper (next) page
		OLED_SendData(BytesToDisplay, Width);                           //Send one page data
		ImageStart += Width;                                            //Increase pointer to data for next page
	}
	return 0;
}

void OLED_DrawPixel(uint8_t x, uint8_t y, uint8_t color)
{
	x += COLLUMN_CENTER_OFFSET;
	if(x > 131 - COLLUMN_CENTER_OFFSET || x < COLLUMN_CENTER_OFFSET || y > DISPLAY_HEIGHT)
	{
		return;
	}
	uint8_t PixelNumber = ( color << (y % 8) );					//Pixel number in 1 byte organized VRAM memory

	OLED_SendCmd( SET_PAGE | (y/8) );							//Set page address, each page has 8 pixels
	OLED_GoToCollumn(x);
	OLED_SendData(&PixelNumber,1);
}

void OLED_DrawLine(uint8_t startX, uint8_t startY, uint8_t lenght)
{
	startX += COLLUMN_CENTER_OFFSET;
	if(startX > 131 - COLLUMN_CENTER_OFFSET || startX < COLLUMN_CENTER_OFFSET || startY > DISPLAY_HEIGHT)
	{
		return;
	}

}

void OLED_DrawLine1(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t color){


    int dx =  abs(x2-x1), sx = x1<x2 ? 1 : -1;
    int dy = -abs(y2-y1), sy = y1<y2 ? 1 : -1;
    int err = dx+dy, e2; /* error value e_xy */

    while(1){
        OLED_DrawPixel(x1, y1, color);
        if (x1==x2 && y1==y2) break;
        e2 = 2*err;
        if (e2 > dy) { err += dy; x1 += sx; } /* e_xy+e_x > 0 */
        if (e2 < dx) { err += dx; y1 += sy; } /* e_xy+e_y < 0 */
    }

}

void DEBUG_Sendi(uint16_t variable, char *String)
{
	uart_sends(String);
	uart_sends(": ");
	uart_sendi(variable);
	uart_sends("\r\n");
}
