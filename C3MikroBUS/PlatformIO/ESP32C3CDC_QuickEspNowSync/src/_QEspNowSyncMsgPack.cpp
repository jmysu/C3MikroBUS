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
//PeerListClass myPeers;

//#define LED_ON  0
//#define LED_OFF 1
#define DEST_ADDR ESPNOW_BROADCAST_ADDRESS 

#include "myEspNow.h" //struct && enum
espnow_struct EspNow;

bool isEspNowRXed=false;
 
uint64_t myMac, macLeader;
//Preferences
#include <Preferences.h>
uint8_t _pinBlink=0;
bool _pinBlinkOn=0;
Preferences myPref;


//-----------------------------------------------------------------------------
#include <Ticker.h>
Ticker tickBlinkLED;
Ticker tickOffLED;
void offLED()
{
    digitalWrite(_pinBlink, !_pinBlinkOn); //OFF LED
}
void blinkLED()
{
    digitalWrite(_pinBlink, _pinBlinkOn); //toggle LED
    tickOffLED.once_ms(100, offLED);    //Off LED 100ms later
 }

bool isUnicastSent = false; 
uint32_t utimeDataSent; //micros starting sent
/**
 * @brief EspNow dataSentCB
 * 
 * @param address 
 * @param status 
 */
void dataSentCB (uint8_t* address,  uint8_t status ) {
    if (ESP_NOW_SEND_SUCCESS==status) {
        isUnicastSent = true;
        utimeDataSent = micros()-utimeDataSent;
        Serial.printf ("Unicast to *%02X%02X:%luus!\n", address[4], address[5], utimeDataSent);
        }
    else    
        Serial.printf ("Unicast to *%02X%02X:FAIL!\n", address[4], address[5]);
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
    if (len<=0) return;
    switch (*data) {
        case myEspNowMsgType::HELLO:
        case myEspNowMsgType::ECHO_REQ:
        case myEspNowMsgType::ECHO_ACK:
            memcpy(&EspNow, data, len);
            EspNow.rssi = rssi;
            {
            uint32_t uOUI= address[0]<<16 | address[1]<<8 | address[2];
            uint32_t uID = address[3]<<16 | address[4]<<8 | address[5];
            EspNow.macAddr64 = ((uint64_t)uOUI) << 24 | uID; //convert OUI|ID into uint64_t
            }
            //Serial.printf("RX peer:%llX time:%lu\n", EspNow.macAddr64, EspNow.ums);
            break;
        case myEspNowMsgType::MSGPACK:
            EspNow.msgType = myEspNowMsgType::MSGPACK;  //copy msgType to EspNow
            EspNow.rssi = rssi;                         //copy rssi to EspNow
            _myMessagePack.unpack(data+1, len-1);       //unpack msg data
            if (0) {
                Serial.print("Rx msgPack:");
                for (int i=0;i<len;i++) {
                    Serial.printf(" %0X", *(data+i));    
                    }
                Serial.println();  
                Serial.print("msgPack:");
                for (int i=0;i<_myMessagePack.length();i++) {
                    Serial.printf(" %0X", _myMessagePack.pack()[i]);    
                    }
                Serial.println();  
                }
            break;
            }     
    isEspNowRXed=true; 
    if (0) {
        Serial.printf("\n[%lu]EspNowRX: Type:0x%0X\n", millis(), *data);    
        Serial.printf("Rx:%u RSSI:%ddBm\n", len, rssi);
        Serial.printf ("From:[" MACSTR "] ", MAC2STR (address));
        Serial.printf ("%s\n", broadcast ? "Broadcast" : "Unicast");
        }
}
void sendEspNowMsgPack() {
    MessagePack *EspMsgPack;
    EspMsgPack = new MessagePack();
    EspMsgPack->addLong(myMac >> 32);
    EspMsgPack->addLong(myMac & 0xFFFFFF);
    EspMsgPack->addLong(millis());
    EspMsgPack->addFloat(readTempSensor());
 
    uint8_t uSize = EspMsgPack->length()+1;
    uint8_t aESP[uSize];
    aESP[0]=myEspNowMsgType::MSGPACK;
    memcpy(aESP+1, EspMsgPack->pack(), EspMsgPack->length());
    //for (int i=0;i<uSize;i++) {
    //    Serial.printf(" %0X", aESP[i]);
    //    }
    //Serial.println(); 
    if (0==quickEspNow.send (DEST_ADDR, (uint8_t*)aESP, uSize ) ) { 
        Serial.printf(">>>>>>>>>>> myEspNowMsgPack sent!\n");  
        } 
    else {
        Serial.printf (">>>>>>>>>> myEspNowMsgPack NOT sent\n");
        }
        
}
void broadcastEspNowHello()
{ 
    espnow_struct myData;    
    myData.msgType = myEspNowMsgType::HELLO;
    myData.macAddr64 = myMac; 
    //myData.frames = counter++;
    myData.ums = millis();
    if (0==quickEspNow.send (DEST_ADDR, (uint8_t*)&myData, sizeof(espnow_struct)) ) {
        //Serial.printf ("[%lu]>>>", millis());
        //Serial.printf("ID:%0X Millis:%lu\n", myData.id, myData.ums);
        macLeader = _myNodesTime->getKey(_myNodesTime->size()-1);
        if (myData.macAddr64 == macLeader) { //Leader do 1000ms blink by himself
            uint32_t iums = myData.ums%1000;
            tickBlinkLED.once_ms(1002-iums, blinkLED); //Scheduling for next blink, with 2ms RTTI adjustment   
            }  
        else {
            //Team member following blink upon ESPNOW RX
            }    
        //Update nodes time    
        if (!_myNodesTime->has(myData.macAddr64)) {     //New member
            _myNodesAlive->put(myData.macAddr64, 0);    //Add 0 to nodesAlive
            _myNodesRSSI->put(myData.macAddr64, 0);     //Add 0 to nodesRSSI
            _myNodesTime->put(myData.macAddr64, myData.ums);
            Serial.printf("<%0llX> Added!\n", myData.macAddr64);
            //printNodesOld();
            //printNodes(); 
            }
        else { //Old member
            _myNodesTime->put(myData.macAddr64, myData.ums);
            //Update RSSI for myself
            _myNodesRSSI->put(myData.macAddr64, 0);  //No talk to myself!
            }
        } 
    else {
        Serial.printf (">>>>>>>>>> Message NOT sent\n");
        }
}
void unicastEspNowEchoReq(uint64_t mac)
{
    espnow_struct myData;    
    myData.msgType = myEspNowMsgType::ECHO_REQ;
    myData.macAddr64 = myMac;
    myData.ums = micros();
    quickEspNow.onDataSent(dataSentCB);
    //convert uint64 to mac array
    uint8_t mArray[6];
    mArray[0]=(mac>>40)&0x00FF;
    mArray[1]=(mac>>32)&0x00FF;
    mArray[2]=(mac>>24)&0x00FF;
    mArray[3]=(mac>>16)&0x00FF;
    mArray[4]=(mac>>8) &0x00FF;
    mArray[5]=mac      &0x00FF;
    
    utimeDataSent= micros();
    if (0==quickEspNow.send (mArray, (uint8_t*)&myData, sizeof(espnow_struct)) ) {
        //Serial.printf ("[%lu]>>>", millis());
        //Serial.printf("ID:%0X Millis:%lu\n", myData.id, myData.ms);   
        //Serial.printf(">>> ECHO_REQ sent!\n");   
        } 
    else {
        Serial.printf(">>> ECHO_REQ NOT sent!\n");
        }
}
void sendEspNowEchoAck()
{
    espnow_struct myData;    
    myData.msgType = myEspNowMsgType::ECHO_ACK;
    myData.macAddr64 = EspNow.macAddr64;
    myData.ums = EspNow.ums;
    if (0==quickEspNow.send (DEST_ADDR, (uint8_t*)&myData, sizeof(espnow_struct)) ) {
        //Serial.printf ("[%lu]>>>", millis());
        //Serial.printf("ID:%0X Millis:%lu\n", myData.id, myData.ms);   
        //Serial.printf(">>> ECHO_ACK sent!\n");   
        } 
    else {
        Serial.printf(">>> ECHO_ACK NOT sent!\n");
        }
}
Ticker tickSendHello;
void espSendHelloCB()
{
    quickEspNow.onDataSent(NULL);
    broadcastEspNowHello();   
}



//=============================================================================
void setup () {
    //pinMode(_pinBlink, OUTPUT);
    
    #if (ARDUINO_USB_CDC_ON_BOOT)
     log_w("USB_CDC_ON_BOOT enabled!");
     if (isUSBCDC()) {
       if (!Serial) {Serial.begin(115200); delay(300);}
       // Workaround for when USB CDC is unplugged #7583 
       Serial.setTxTimeoutMs(0);   // <<<====== solves USBCDC issue of delay
       }
    #else
     log_w("USB_CDC_ON_BOOT disabled!");
     if (!Serial) {Serial.begin(115200); delay(300);}       
    #endif

    printESP32info();
  
    //Save pinBlink to NVS::my-app
    //----------------------------
    //myPref.begin("my-app", false); //ReadWrite mode
    //_pinBlink = 3;
    //_pinBlinkOn=1;
    //myPref.putChar("pinBlink", _pinBlink); //reset pinBlink
    //myPref.putChar("pinBlinkOn", _pinBlinkOn);

    //Read pinBlink from NVS::my-app
    //------------------------------
    myPref.begin("my-app", true); //ReadOnly mode
    _pinBlink=myPref.getChar("pinBlink", 0);
    _pinBlinkOn=myPref.getChar("pinBlinkOn", 0);
    
    Serial.printf("NVS pinBlink:%d\n", _pinBlink);
    Serial.printf("NVS pinBlinkOn:%d\n", _pinBlinkOn);
    
    myPref.end();
    pinMode(_pinBlink, OUTPUT); //reset pinBlink as OUTPUT
    
    WiFi.mode(WIFI_MODE_STA);
    /* Config LR Mode
        For LR-enabled AP of ESP32, it is incompatible with traditional 802.11 mode, because the beacon is sent in LR mode.
        For LR-enabled station of ESP32 whose mode is NOT LR-only mode, it is compatible with traditional 802.11 mode.
        If both station and AP are ESP32 devices and both of them have enabled LR mode, the negotiated mode supports LR.*/
    esp_wifi_set_protocol( WIFI_IF_STA, WIFI_PROTOCOL_LR ); //11B/11G/11N/LR
    //
    WiFi.disconnect(false);
    //quickEspNow.onDataSent(dataSentCB);
    quickEspNow.onDataRcvd(dataReceivedCB);
    quickEspNow.begin(1); // If you are not connected to WiFi, channel should be specified
    tickSendHello.attach_ms(1000, espSendHelloCB); //Send HELLO every 1000ms
   
    //myId=getESP32ChipID24(); //get my ID
    //myMac = ESP.getEfuseMac();
    //Serial.printf("EfuseMAC:%012X", myMac);
    uint8_t mac[6];
    WiFi.macAddress(mac);
    Serial.printf("WiFiMAC:" MACSTR "\n", MAC2STR(mac));
    uint32_t uOui = mac[0]<<16 | mac[1]<<8 | mac[2];
    uint32_t uId  = mac[3]<<16 | mac[4]<<8 | mac[5];
    myMac = ((uint64_t)uOui)<<24 | uId;
    macLeader = myMac;
    //Serial.printf("uOui Id:%X:%X", uOui, uId);
    Serial.printf("My MAC address:%llX\n", myMac);
    

    setupNodesMap();
    _myNodesAlive->clear();
    _myNodesTime->put(myMac, millis());
    printNodesAlive();
    //=====================================
    
}

uint32_t lastStatusMs=0; 
uint8_t counter=0;
void loop () {
    //print Nodes & check KeepAlive every 3seconds
    if ((millis()-lastStatusMs)>=3000) {
        lastStatusMs = millis(); //update lastStatus counter
        //printNodesOld();
        //printNodesAlive();
        //Check KeepAlive, drop node when timeDiff<1000ms
        for (int i=0;i<_myNodesTime->size();i++) { 
            uint64_t mac=_myNodesTime->getKey(i); //get key
            uint32_t timeOld = _myNodesAlive->getData(_myNodesAlive->getIndex(mac)); //find key/value in alive nodes
            uint32_t timeNew = _myNodesTime->getData(i);
            uint32_t timeDiff= timeNew - timeOld;
           
            _myNodesAlive->put(mac, timeNew); //update NodesAlive w/ new time
            //Serial.printf(" %0llX:%lu",  mac, timeDiff);
            if (timeDiff<1000){
                _myNodesTime->remove(_myNodesTime->getIndex(mac));
                _myNodesAlive->remove(_myNodesAlive->getIndex(mac));
                _myNodesRSSI->remove(_myNodesRSSI->getIndex(mac));
                Serial.printf("<%0llX> Dropped!\n", mac);
                }
            }   
        printNodesRSSI();    
        //printNodes();
        //Serial.println();
        //sendEspNowEchoReq(); //request a HELLO ECHO!
        #if (ENABLE_MESSAGEPACK)
         sendEspNowMsgPack(); 
        #endif
        #if (ENABLE_UNICAST)
         //unicast to members in myNodes
         if (counter <_myNodesTime->size()) {
            uint64_t mac = _myNodesTime->getKey(counter);
            if (mac != myMac)       
                unicastEspNowEchoReq(mac);
            counter++;    
            }
         else counter=0;    
        #endif
        }
    if (isEspNowRXed) { //Got EspNow RX data?
        isEspNowRXed=false;
        switch (EspNow.msgType) {
            case myEspNowMsgType::HELLO: 
                //idLeader is the last one in _myNodesTime
                macLeader = _myNodesTime->getKey(_myNodesTime->size()-1);
                if (EspNow.macAddr64 == macLeader) { //Leader's HELLO
                    tickBlinkLED.once_ms(1000-(EspNow.ums%1000), blinkLED); //Scheduling for next blink and try to sync w/ Leader   
                    }
                if (!_myNodesTime->has(EspNow.macAddr64)) {     //New memeber
                    _myNodesAlive->put(EspNow.macAddr64, 0);    //Add 0 to nodesAlive
                    _myNodesRSSI->put(EspNow.macAddr64, 0);     //Add 0 to nodesRSSI
                    _myNodesTime->put(EspNow.macAddr64, EspNow.ums);
                    Serial.printf("<%0llX> Added!\n", EspNow.macAddr64);
                    //printNodesOld();
                    //printNodes();
                    }
                else  {  //Old member
                    _myNodesTime->put(EspNow.macAddr64, EspNow.ums);  
                    //Average RSSI for node
                    int8_t rssiOld = _myNodesRSSI->getData(_myNodesRSSI->getIndex(EspNow.macAddr64));
                    //Serial.printf("<%0llX>RSSI:%d/%d\n",  EspNow.macAddr64, rssiOld, EspNow.rssi);
                    _myNodesRSSI->put(EspNow.macAddr64, (EspNow.rssi/2 + rssiOld/2));
                    }
                break; 
            case myEspNowMsgType::ECHO_REQ:
                //sendEspNowEchoAck(); //Ack the REQ, send back HELLO
                //break;
            case myEspNowMsgType::ECHO_ACK: //Echo acked! calculate RTTI 
                if (myMac == EspNow.macAddr64) {
                    uint32_t uRTTI = micros() - EspNow.ums;
                    Serial.printf("!!! ECHO RTTI=%luus from:%llX (%d)dBm\n", uRTTI, EspNow.macAddr64, EspNow.rssi);
                    //Serial.printf(" From mac:%02x:%02x:%02x:%02x:%02x:%02x\n", MAC2STR(EspNow.mac));
                    }
                break;           
            case myEspNowMsgType::MSGPACK:
                parseMessagePack(_myMessagePack.pack());
                break;    
                }    
            }
}