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

#define DISPLAY_ON 			0xAF		//display ON
#define DISPLAY_OFF 		0xAE		//display OFF (sleep)
#define SET_PAGE 			0xB0		//Set page address 0-7
#define SET_COLLUMN_ADDR_L	0x00		//Set lower part off collumn address byte (0x00 - 0x0F)
#define SET_COLLUMN_ADDR_H	0x10		//Set higher part off collumn address byte (0x10 - 0x1F)
#define SET_VOLTAGE			0x30		//Set voltage of internal charger pump (0x30 - 0x33)
#define SET_START_LINE		0x40		//Specifies line address to the initial display line (0x40 - 0x7F)
#define SET_CONTRAST		0x81		//Double bytes command! After command, send contrast value (0x00 - 0xFF)
#define SET_SEGMENT_REMAP	0xA0		//Segment re-map (collumn (x) addressing start from the bottom or the top of display) (0xA0 or 0xA1)
#define SET_INVERSE_MODE	0xA7		//Inverse display mode
#define SET_NORMAL_MODE		0xA6		//Non-inverse display mode
#define SET_MULTIPLEX_RATIO	0xA8		//Double bytes command! Set multiplex ratio (display height) (0x00 - 0x3F)
#define SET_DCDC			0xAD		//Double bytes command! (0x8A - disable, 0x8B - enable)
#define SET_COMM_SCAN_DIR	0xC0		//Common pins scan direction (page (y) addressing starting from bottom or from top) (0xC0 or 0xC8)
#define SET_OFFSET			0xD3		//Double bytes command!









#endif /* OLED_DEFS_H_ */
