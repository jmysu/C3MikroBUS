#include <Arduino.h>
#include <Preferences.h>
#include "_main.h"

extern Preferences myPref;
//extern void backtoFactory();
extern void backtoApp();

struct Button {
  const uint8_t PIN;
  uint32_t numberKeyPresses;
  bool pressed;
};

Button btnBoot = {pinBoot, 0, false};

void IRAM_ATTR isr() {
  btnBoot.numberKeyPresses += 1;
  btnBoot.pressed = true;
}

void setupButtonBoot() {
  //Serial.begin(115200);
  pinMode(btnBoot.PIN, INPUT_PULLUP);
  attachInterrupt(btnBoot.PIN, isr, FALLING);
}

void loopButtonBoot() {
  if (btnBoot.pressed) {
      detachInterrupt(btnBoot.PIN);
      delay(500);
      #if defined(PIN_C3mBUS0)
       neopixelRGB(32,0,0); 
      #endif  
      
      if (digitalRead(pinBoot)==0) { 
        Serial.println("Button BOOT long pressed! BackToAPP!!!");
        btnBoot.numberKeyPresses = 0;
        if (_isPwmBuzzer) Play_DingDong();

        myPref.begin("my-app", false); //ReadWrite
        myPref.putBool("OTA", false);
        myPref.end();
        delay(100);
 
        //backtoFactory(); //Set factory bootable and restart!
        backtoApp(); //
        }
      else
        Serial.printf("Button BOOT pressed %u times\n", btnBoot.numberKeyPresses);
      
      btnBoot.pressed = false;
      attachInterrupt(btnBoot.PIN, isr, FALLING);
    }
}