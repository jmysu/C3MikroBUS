#ifndef MY_ESPNOW_H
#define MY_ESPNOW_H
#include <Arduino.h>
#include <SimpleMap.h>

#define LED0 0

enum myEspNowMsgType {
  HELLO       =0x10, // ID+frames+millis
  SYNC_REQ    =0x80, // Request to sync
  SYNC_ACK,  
  Reserved   // Reserved
};

struct espnow_struct {
    uint8_t   msgType;  //msgType: 0x10:time-report, 0x18:echo time-report
    uint32_t  id;
    uint16_t  frames;
    uint32_t  ms;
    int8_t    rssi;
};
 

extern uint32_t getESP32ChipID24();
extern void neopixelRGB(uint8_t r, uint8_t g, uint8_t b);
extern void neopixelSetupBlink(int msBlink);
extern void printESP32info();

//extern SimpleList<uint32_t>* listID;
//extern void printListID();
//extern void setupListID();
 
extern SimpleMap<uint32_t, uint32_t> *_myNodes, *_myNodesOld;
extern void setupNodesMap();
extern void printNodes();
extern void printNodesOld();


//WiFi Promiscuous Sniff
//extern void setupWiFiSniffer();

#endif