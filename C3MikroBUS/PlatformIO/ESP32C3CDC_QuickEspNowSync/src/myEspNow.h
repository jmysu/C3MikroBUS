#ifndef MY_ESPNOW_H
#define MY_ESPNOW_H
#include <Arduino.h>
#include <SimpleMap.h>

#define LED0 0

#include <MessagePack.h>
extern MessagePack _myMessagePack;

enum myEspNowMsgType {
  HELLO       =0x10, // ID+frames+millis
  ECHO_REQ,          // HELLO ECHO request, send HELLO packets back! 
  ECHO_ACK,          // HELLO ECHO ackowledge
  MSGPACK     =0x80, // MsgPack Data
   
  Reserved   // Reserved
};

struct espnow_struct {
    uint8_t   msgType;  //msgType: 0x10:time-report, 0x18:echo time-report
    uint64_t  macAddr64;
    //uint16_t  frames;
    uint32_t  ums;
    int8_t    rssi;
};

extern bool isUSBCDC();
extern uint32_t getESP32ChipID24();
extern void neopixelRGB(uint8_t r, uint8_t g, uint8_t b);
extern void neopixelSetupBlink(int msBlink);
extern void printESP32info();

//extern SimpleList<uint32_t>* listID;
//extern void printListID();
//extern void setupListID();

extern uint64_t myMac; 
extern SimpleMap<uint64_t, uint32_t> *_myNodesTime, *_myNodesAlive;
extern SimpleMap<uint64_t, int8_t> *_myNodesRSSI;
extern void setupNodesMap();
extern void printNodesTime();
extern void printNodesOld();
extern void printNodesAlive();
extern void printNodesRSSI();

//WiFi Promiscuous Sniff
//extern void setupWiFiSniffer();

extern float readTempSensor();
extern void setupTempSensor();

extern void parseMessagePack(unsigned char * data);
#endif