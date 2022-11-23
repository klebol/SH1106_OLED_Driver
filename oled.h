/*
 * oled.h
 *
 *  Created on: 13 lis 2022
 *      Author: miqix
 */

#ifndef OLED_H_
#define OLED_H_


typedef struct{
	uint8_t collumn;	// X
	uint8_t page;		// Y/8
}CursorPosition;


void OLED_SendCmd(uint8_t command);
void OLED_Send2ByteCmd(uint8_t command, uint8_t value);
void OLED_SendData(uint8_t* Data, uint8_t length);
void OLED_DrawPixel(uint8_t x, uint8_t y, uint8_t color);
void OLED_Init(void);
void OLED_GoToCollumn(uint8_t CollumnNumber);
void OLED_ClearDisp(void);
void OLED_WriteChar(char character);
void OLED_MoveCursor(uint8_t collumn, uint8_t page);
void OLED_DrawLine1(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t color);




#endif /* OLED_H_ */
