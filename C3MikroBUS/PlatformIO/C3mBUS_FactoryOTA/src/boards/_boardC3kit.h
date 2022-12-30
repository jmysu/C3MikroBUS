/*
 * BOARD.H
 */
#ifndef _BOARD_H
#define _BOARD_H

//Board-----------------
//#define BOARD_C3KIT
//#define BOARD_C3mikroBUS
//#define BOARD_C3pico
#pragma message (BOARDNAME)

//SDA/SCL are different w/ C3pico/C3mikroBUS 
#define pinSDA  9
#define pinSCL  8

#define pinBoot 9

#define pinTouchRx  6
#define pinTouchTx  7

//#define pinNeopixel 4
//#define pinBuzzer   5

#define pinLynxRx   3
#define pinLynxTx   1
#define pinBuzzer   18 //D-
#define pinNeopixel 19 //D+

/*
//for C3-12F-Kit
#define pinLED_R     3
#define pinLED_G     4
#define pinLED_B     5
#define pinAD0       0
#define pinLED_Warm  18
#define pinLED_Cold  19
*/
#endif
