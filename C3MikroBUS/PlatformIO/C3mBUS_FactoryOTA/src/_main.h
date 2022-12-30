/*
        ESP-32-C3-12F Kit Touchscreen-Lynx converter

                        ┌────────────────┐
                        │     ┌──────┐   │
                AD0 | 01│●  ┌─┘      │  ●│30 | IO19 | LED-Cold | Neopixel
     LynxTx <   IO1 | 02│●  │        │  ●│29 | IO18 | LED-Warm | Buzzer
                NC  | 03│   └────────┘  ●│28 | IO10 
                IO2 | 04│● ┌─────┐19 18 ●│27 | IO9  | I2C SDA  | BOOTKEY
                IO3 | 05│● │3/4/5│⏄ ⏄  ●│26 | IO8  | I2C SCL
                IO4 | 06│● └─────┘      ■│25 | 3V3
                IO5 | 07│●              ■│24 | GND
                NC  | 08│                │23 | NC
                NC  | 09│               ●│22 | IO6  < TouchRx
                GND | 10│■              ●│21 | IO7  > TouchTx
                3V3 | 11│■               │20 | NC
                EN  | 12│               ●│19 | IO20 < USB-RX
     LynxRx >   IO0 | 13│●              ●│18 | IO21 > USB-TX
                GND | 14│■              ■│17 | GND
                5V  | 15│■    ┌────┐    ■│16 | 3V3
                        └─────┃    ┃─────┘          
                              ┗━━━━┛
=============================================================================
                      Buzzer:18  Neopixel:19
                        ┌─────C3Pico─────┐
        LynxRX |AD0 | 01│●AN0       1PWM●│16 | IO1  | LynxTx
                    | 02│●RST       3INT●│15 | IO3
    TouchTx7 IO7/10 | 03│●SS7/10     RX ●│14 | Rx 
                IO4 | 04│●SK4        TX ●│13 | Tx 
       TouchRx6 IO6 | 05│●SO6       9SCL●│12 | IO9  | I2C SCL |BOOT
                IO5 | 06│●SI5       8SDA■│11 | IO8  | I2C SDA 
                      07│●3v3        +5V■│10
                      08│●GND        GND■│09  
                        └────────────────┘     
=============================================================================
                       Buzzer:5  Neopixel:4
                        ┌──-C3MikroBUS───┐
                AD0 | 01│●AN0       1PWM●│16 | IO1  | LynxTx
                    | 02│●RST       3INT●│15 | IO3  | LynxRx
             IO7/10 | 03│●SS7/10     RX ●│14 | Rx 
       Neopixel IO4 | 04│●SK4        TX ●│13 | Tx 
       TouchRx6 IO6 | 05│●SO6       9SCL●│12 | IO9  | I2C SCL |BOOT
         Buzzer IO5 | 06│●SI5       8SDA■│11 | IO8  | I2C SDA 
                      07│●3v3        +5V■│10
                      08│●GND        GND■│09  
                        └────────────────┘     
Booting Mode 
Pin   Default         SPI Boot      Download Boot
--------------------------------------------------
GPIO2 N/A               1           1
GPIO8 N/A               Don’t care  1
GPIO9 Internal-pull-up  1           0

*/
#ifndef _MAIN_H
#define _MAIN_H
#include <Arduino.h>

#if PIN_C3mBUS0
 #include "boards/_boardC3mikroBUS_V0.h"
#elif PIN_C3mBUSpico
 #include "boards/_boardC3pico.h"
#endif

extern bool isUSBCDC();
extern uint32_t getESP32ChipID24();
extern void printESP32info();


extern bool _isPwmBuzzer;
extern void setupPwmBuzzer();
extern void pwmBuzzer(int8_t duty, int freq, int durationMills);
extern void Play_DingDong();
extern void Play_DongDing();
extern void Play_Pacman();
extern void Play_CrazyFrog();
extern void Play_Titanic();
extern void Play_Pirates();
extern void Play_MarioUW();
extern void Play_MarioOver(); 
extern void Play_135();
extern void Play_531();
extern void Beep(uint8_t duty, int duration);
extern void Play_RampUp();
extern void Play_RampDown();

extern void neopixelSetRGBw();
extern void neopixelSetRGBcolor(int index, uint8_t r, uint8_t g, uint8_t b);
extern void neopixelRGB(uint8_t r, uint8_t g, uint8_t b);
extern void neopixelSetupBlink(int msBlink);

extern void setupWiFiEvents();
extern bool _isAPconnected;

extern void setupButtonBoot();
extern void loopButtonBoot();

extern void setupEspNow();
extern void loopEspNow();

#endif