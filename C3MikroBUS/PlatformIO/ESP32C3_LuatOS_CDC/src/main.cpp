/**
 * @file main.cpp
 * @author jimmy.su
 * @brief 
 * @version 0.1
 * @date 2022-11-05
 * 
 * @copyright Copyright (c) 2022
 * 
 * 
 * Remember to copy loatos-cdc.json to ./platformio/platforms/boards
 * 
 */
#include <Arduino.h>
#include "HWCDC.h"


void setup() {
  // put your setup code here, to run once:
  //pinMode(9, INPUT_PULLUP);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);

  Serial.begin(115200);
  Serial.setDebugOutput(true); //Enable log to CDC serial
  while (!Serial) {}
  delay(2000);

  log_w("====================");
  log_d("ESP32C3 USBCDC Test!");
  log_w("====================");
  Serial.print(F("Build Time: "));
  Serial.print(F(__DATE__));
  Serial.print(F("  "));
  Serial.print(F(__TIME__));
  Serial.println();

}

int i=0;
void loop() {
  
  // put your main code here, to run repeatedly:
  digitalWrite(12, HIGH);
  digitalWrite(13, LOW);
  delay(1000);
  digitalWrite(12, LOW);
  digitalWrite(13, HIGH);
  delay(1000);
  
  i++;
  Serial.printf("USBCDC:%d\n",i);
}