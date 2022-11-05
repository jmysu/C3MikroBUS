#include <Arduino.h>
#include "HWCDC.h"


void setup() {
  // put your setup code here, to run once:
  //pinMode(9, INPUT_PULLUP);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);

  Serial.begin(115200);
  delay(500);
 
  Serial.println("\nESP32C3 USBCDC Test!");
  Serial.print(F(__DATE__));
  Serial.print(F("  "));
  Serial.print(F(__TIME__));
  Serial.println();
  Serial.println("=======================");

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