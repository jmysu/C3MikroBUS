/**
 * @file _QEspNowSync.cpp
 * @author jimmy.su
 * @brief  QEspNow LED Sync
 *         Dependancy library:QuickEspNow, SimpleMap, Esp32Ticker
 * @version 0.1
 * @date 2023-01-06
 * 
 * @copyright Copyright (c) 2023
 *  
 */
#include <Arduino.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include "QuickEspNow.h"

#define LED_ON  0
#define LED_OFF 1
#define DEST_ADDR ESPNOW_BROADCAST_ADDRESS 

#include "myEspNow.h" //struct && enum
espnow_struct EspNow;
bool isEspNowRXed=false;

uint32_t idLeader, idLeaderOld, myId;  //id of Team leader

//-----------------------------------------------------------------------------
#include <Ticker.h>
Ticker tickBlinkLED;
Ticker tickOffLED;
void offLED()
{
    digitalWrite(LED0, LED_OFF); //OFF LED
}
void blinkLED()
{
    digitalWrite(LED0, LED_ON); //toggle LED
    tickOffLED.once_ms(100, offLED);    //Off LED 100ms later
 }

/**----------------------------------------------------------------------------
 * @brief EspNow dataReceivedCB
 *          
 * @param address 
 * @param data 
 * @param len 
 * @param rssi 
 * @param broadcast 
 */
void dataReceivedCB (uint8_t* address, uint8_t* data, uint8_t len, signed int rssi, bool broadcast) {
    if (len>0) {
        memcpy(&EspNow, data, len);
        EspNow.rssi = rssi;
        isEspNowRXed=true; 
        //Serial.printf("\n[%lu]EspNowRX: Type:0x%0X\n", millis(), *data);
        }
    //Serial.printf("Rx:%u RSSI:%ddBm\n", len, rssi);
    //Serial.printf ("From:[" MACSTR "] ", MAC2STR (address));
    //Serial.printf ("%s\n", broadcast ? "Broadcast" : "Unicast");
}

void sendEspNowHello()
{ 
    espnow_struct myData;    
    myData.msgType = myEspNowMsgType::HELLO;
    myData.id = myId;
    //myData.frames = counter++;
    myData.ms = millis();
    if (0==quickEspNow.send (DEST_ADDR, (uint8_t*)&myData, sizeof(espnow_struct)) ) {
        //Serial.printf ("[%lu]>>>", millis());
        //Serial.printf("ID:%0X Millis:%lu\n", myData.id, myData.ms);
        //Check if I am the leader!
         idLeader = _myNodes->getKey(_myNodes->size()-1);
         if (myData.id == idLeader) { //Leader's HELLO
            int ims= myData.ms%1000;
            tickBlinkLED.once_ms(1000-ims, blinkLED); //Scheduling for next blink   
            }  
        //Copy old time into myNodesOld
        _myNodesOld->put(myData.id, _myNodes->getData(_myNodesOld->getIndex(myData.id)));    
        //Update myNodes with new time
        _myNodes->put(myData.id, myData.ms);         
        } 
    else {
        Serial.printf (">>>>>>>>>> Message NOT sent\n");
        }
}
Ticker tickSendHello;
void espSendHelloCB()
{
    sendEspNowHello();    
}

//=============================================================================
void setup () {
    pinMode(LED0, OUTPUT);
    Serial.begin (115200);
    // Workaround for when USB CDC is unplugged #7583 
    Serial.setTxTimeoutMs(0);   // <<<====== solves USBCDC issue of delay
    delay(500);
    printESP32info();

    WiFi.mode(WIFI_MODE_STA);
    WiFi.disconnect(false);
    quickEspNow.onDataRcvd(dataReceivedCB);
    quickEspNow.begin(1); // If you are not connected to WiFi, channel should be specified
    tickSendHello.attach_ms(1000, espSendHelloCB); //Send HELLO every 1000ms

    myId=getESP32ChipID24(); //get my ID
    setupNodesMap();
    printNodes();
}

uint32_t lastStatusMs=0; 
void loop () {
    //print Nodes & check KeepAlive every 3seconds
    if ((millis()-lastStatusMs)>=3000) {
        lastStatusMs = millis(); 
        //printNodesOld();
        for (int i=0;i<_myNodes->size();i++) { 
            uint32_t timeDiff = _myNodes->getData(i)-_myNodesOld->getData(i);
            uint32_t id=_myNodes->getKey(i);
            uint32_t time=_myNodes->getData(i);
            _myNodesOld->put(id, time); //copy new time into _myNodesOld
            //Serial.printf(" %lu",  timeDiff);
            if (timeDiff<1000){
                _myNodes->remove(i);
                _myNodesOld->remove(i);
                Serial.printf("<%0X> Dropped!\n", id);
                }
            }
        //Serial.println();    
        printNodes();
        }
    if (isEspNowRXed) { //Got EspNow RX data?
        isEspNowRXed=false;
        switch (EspNow.msgType) {
            case myEspNowMsgType::HELLO: 
                //idLeader is the last one in _myNodes
                idLeader = _myNodes->getKey(_myNodes->size()-1);
                if (EspNow.id == idLeader) { //Leader's HELLO
                    int ims= EspNow.ms%1000;
                    tickBlinkLED.once_ms(1000-ims, blinkLED); //Scheduling for next blink   
                    }
                //Check new member!
                if (!_myNodes->has(EspNow.id)) Serial.printf("<%0X> Added!\n", EspNow.id);

                //update myNodesOld time
                _myNodesOld->put(EspNow.id, _myNodes->getData(_myNodesOld->getIndex(EspNow.id)));  
                //update myNodes
                _myNodes->put(EspNow.id, EspNow.ms);
                //printNodes();
                break;    
                }    
        }
}