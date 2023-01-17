
#include <Arduino.h>
#include <MessagePack.h>
 
MessagePack _myMessagePack; //MessagePack for rx

void parseMessagePack(unsigned char * data) {
    //MessagePack * message = new MessagePack();
    Serial.print("myMessagePack:");
    for (int i=0;i<_myMessagePack.length();i++) {
        Serial.printf(" %02X", _myMessagePack.pack()[i]);
        }
    Serial.println();    
    // Load the data
    //message->unpack(data, message->length());
    // Number of elements
    unsigned char count = _myMessagePack.count();
    //Serial.print("Number of values: ");
    //Serial.println(count);
    // Walk elements
    Serial.print("("); 
    char buffer[32];
    for (unsigned char i=0; i<count; i++) {
        unsigned char type = _myMessagePack.type(i);
        if (MESSAGE_PACK_BOOLEAN == type) {
            snprintf(buffer, sizeof(buffer), " %d", _myMessagePack.getBoolean(i) ? 1 : 0);
        } else if (MESSAGE_PACK_SHORT == type) {
            snprintf(buffer, sizeof(buffer), " %u", _myMessagePack.getShort(i));
        } else if (MESSAGE_PACK_INTEGER == type) {
            snprintf(buffer, sizeof(buffer), " %u", _myMessagePack.getInteger(i));
        } else if (MESSAGE_PACK_LONG == type) {
            snprintf(buffer, sizeof(buffer), " %0X", _myMessagePack.getLong(i));
        } else if (MESSAGE_PACK_FLOAT == type) {
            snprintf(buffer, sizeof(buffer), " %.2f", _myMessagePack.getFloat(i));    
        }
        Serial.print(buffer);
        }
    Serial.println(")");    
}