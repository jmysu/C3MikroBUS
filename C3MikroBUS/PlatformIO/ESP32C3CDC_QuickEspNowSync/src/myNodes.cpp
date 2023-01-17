
#include <Arduino.h>
#include <SimpleMap.h>
SimpleMap<uint64_t, uint32_t> *_myNodesTime, *_myNodesAlive;
SimpleMap<uint64_t, int8_t> *_myNodesRSSI;
#include "myEspNow.h"

void setupNodesMap()
{
 _myNodesTime = new SimpleMap<uint64_t, uint32_t>([](uint64_t &a, uint64_t &b) ->int {
        if (a == b) return 0;      // a and b are equal
        else if (a > b) return 1;  // a is bigger than b
        else return -1;            // a is smaller than b
    });
 _myNodesAlive = new SimpleMap<uint64_t, uint32_t>([](uint64_t &a, uint64_t &b) ->int {
        if (a == b) return 0;      // a and b are equal
        else if (a > b) return 1;  // a is bigger than b
        else return -1;            // a is smaller than b
    });
 _myNodesRSSI = new SimpleMap<uint64_t, int8_t>([](uint64_t &a, uint64_t &b) ->int {
        if (a == b) return 0;      // a and b are equal
        else if (a > b) return 1;  // a is bigger than b
        else return -1;            // a is smaller than b
    });   
 }

void printNodesTime()
{
    Serial.printf("[%lu]===>%d[", millis(), _myNodesTime->size());
    for (int i=0;i<_myNodesTime->size();i++) {
        Serial.printf(" %0llX:%lu",_myNodesTime->getKey(i), _myNodesTime->getData(i)/1000);
        }
    int32_t imsLeader= _myNodesTime->getData(_myNodesTime->size()-1)%1000;  
    Serial.println("]<===");
    //Serial.printf("]<=== %03lu(%03lu)\n", imsLeader, 1000-imsLeader);    
}

void printNodesOld()
{
    Serial.printf("[%lu]--->%d(", millis(), _myNodesAlive->size());
    for (int i=0;i<_myNodesAlive->size();i++) {
        Serial.printf(" %0llX:%lu",_myNodesAlive->getKey(i), _myNodesAlive->getData(i)/1000);
        }
    Serial.printf(")<---\n");    
}

/**
 * @brief print Nodes && KeepAlive time
 * 
 */
void printNodesAlive()
{
    Serial.printf("[%lu]>>>%d[", millis(), _myNodesTime->size());
    for (int i=0;i<_myNodesTime->size();i++) { //print macId:alive seconds
        uint64_t mac = _myNodesTime->getKey(i);
        char cMark = '*';
        if (mac == myMac) cMark='#'; 
        Serial.printf(" %c%04llX:%3lu", cMark, mac&0x00FFFF, (_myNodesTime->getData(i)-_myNodesAlive->getData(i))/1000);
        }
    int32_t imsLeader= _myNodesTime->getData(_myNodesTime->size()-1)%1000;  
    Serial.printf("]<<< %03lu(%03lu)ms\n", imsLeader, 1000-imsLeader); //blink millis   
} 

void printNodesRSSI()
{
    Serial.printf("[%lu]\t>>>%d[", millis(), _myNodesTime->size());
    for (int i=0;i<_myNodesRSSI->size();i++) { //print macId:alive seconds
        uint64_t mac = _myNodesRSSI->getKey(i);
        char cMark = '*';
        if (mac == myMac) cMark='#'; 
        Serial.printf(" %c%04llX:%03d", cMark, mac&0x00FFFF, (mac==myMac)? 0:_myNodesRSSI->getData(i));
        }
    int32_t imsLeader= _myNodesTime->getData(_myNodesTime->size()-1)%1000;  
    Serial.printf("]<<< \tSync:%03lu(%03lu)ms\n", imsLeader, 1000-imsLeader); //blink millis   
} 
