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
	uint8_t line; 		// Y
	uint8_t page;		// Y/8
}CursorPosition;

typedef struct{
	const uint8_t *FontPointer;
	uint8_t FirstChar;
	uint8_t FontWidth;
	uint8_t FontHeight;
}FontInfo;


void OLED_SendCmd(uint8_t command);
void OLED_Send2ByteCmd(uint8_t command, uint8_t value);
void OLED_SendData(uint8_t* Data, uint16_t length);
void OLED_DrawPixel(uint8_t x, uint8_t y, uint8_t color);
void OLED_Init(void);
void OLED_GoToCollumn(uint8_t CollumnNumber);
void OLED_ClearDisp(void);
void OLED_WriteC(char character);
void OLED_MoveCursor(uint8_t collumn, uint8_t line);
void OLED_DrawLine1(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t color);
void OLED_ChangeFont(const uint8_t *Font);
void OLED_WriteS(char *String);
void OLED_GetCursorPosition(CursorPosition *OutputPosition);
void OLED_WriteI(int Value);
uint8_t OLED_DrawBitmapFlash(uint8_t x, uint8_t y, const uint8_t *bitmap);


void DEBUG_Sendi(uint16_t variable, char *String);




#endif /* OLED_H_ */
