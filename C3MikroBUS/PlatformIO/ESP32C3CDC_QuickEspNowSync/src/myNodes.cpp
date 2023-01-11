
#include <Arduino.h>
#include <SimpleMap.h>
SimpleMap<uint32_t, uint32_t> *_myNodes, *_myNodesOld;

void setupNodesMap()
{
 _myNodes = new SimpleMap<uint32_t, uint32_t>([](uint32_t &a, uint32_t &b) ->int {
        if (a == b) return 0;      // a and b are equal
        else if (a > b) return 1;  // a is bigger than b
        else return -1;            // a is smaller than b
    });
 _myNodesOld = new SimpleMap<uint32_t, uint32_t>([](uint32_t &a, uint32_t &b) ->int {
        if (a == b) return 0;      // a and b are equal
        else if (a > b) return 1;  // a is bigger than b
        else return -1;            // a is smaller than b
    });
 }

void printNodes()
{
    Serial.printf("[%lu]===>[", millis());
    for (int i=0;i<_myNodes->size();i++) {
        Serial.printf(" %0X:%lu",_myNodes->getKey(i), _myNodes->getData(i));
        }
    Serial.printf("]<===\n");    
}

void printNodesOld()
{
    Serial.printf("--->(");
    for (int i=0;i<_myNodesOld->size();i++) {
        Serial.printf(" %0X:%lu",_myNodesOld->getKey(i), _myNodesOld->getData(i));
        }
    Serial.printf(")<---\n");    
}