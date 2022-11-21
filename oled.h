/*
 * oled.h
 *
 *  Created on: 13 lis 2022
 *      Author: miqix
 */

#ifndef OLED_H_
#define OLED_H_





void OLED_SendCmd(uint8_t command);
void OLED_Send2ByteCmd(uint8_t command, uint8_t value);
void OLED_SendData(uint8_t* Data, uint8_t length);
void OLED_DrawPixel(uint8_t x, uint8_t y, uint8_t color);
void OLED_Init(void);




#endif /* OLED_H_ */
