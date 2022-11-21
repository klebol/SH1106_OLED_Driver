/*
 * oled_defs.h
 *
 *  Created on: 13 lis 2022
 *      Author: miqix
 */

#ifndef OLED_DEFS_H_
#define OLED_DEFS_H_

#define OLED_ADDR (0x3C << 1) //Without bit - shift 0x1E addres was being sent (idk why)


#define DISPLAY_WIDTH        128
#define DISPLAY_HEIGHT        64

#if DISPLAY_WIDTH == 128
#define COLLUMN_CENTER_OFFSET 2 //For displays with 128p width (SSD1106 is 132x64 driver, when display has 128p pixels are centered in RAM)
#elif DISPLAY_WIDTH == 132
#define COLLUMN_CENTER_OFFSET 0
#endif

//
//CONTROL BYTES
//
#define SEND_COMMAND			0x00
#define SEND_DATA				0x40
#define NOT_LAST_CONTROL_BYTE	(1<<7)


//
//OLED COMMANDS
//

#define SET_PAGE 0xB0








#endif /* OLED_DEFS_H_ */
