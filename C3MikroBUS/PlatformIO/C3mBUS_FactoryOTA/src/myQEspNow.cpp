#include <Arduino.h>

#include <WiFi.h>
#include <esp_wifi.h>
#include <QuickEspNow.h>

static const String msg = "Hello esp-now!";

//static uint8_t receiver[] = { 0x12, 0x34, 0x56, 0x78, 0x90, 0x12 };
//#define DEST_ADDR receiver
#define DEST_ADDR ESPNOW_BROADCAST_ADDRESS 

static const uint8_t CHANNEL = 1;

void dataReceived (uint8_t* address, uint8_t* data, uint8_t len, signed int rssi, bool broadcast) {
    Serial.print ("ESPNOW Received: ");
    Serial.printf ("%.*s\n", len, data);
    Serial.printf ("RSSI: %d dBm\n", rssi);
    Serial.printf ("From: " MACSTR " " , MAC2STR(address));
    Serial.printf ("%s\n", broadcast ? "Broadcast" : "Unicast");
}

void setupEspNow() {
    //Serial.begin (115200);
    //if (!WiFi.mode (WIFI_MODE_AP)) {
    //    Serial.println ("WiFi mode not supported");
    //}
    //if (!WiFi.softAP ("espnowAP", "1234567890", CHANNEL)) {
    //    Serial.println ("WiFi access point not started");
    //}

    Serial.printf ("Started AP %s in channel %d\n", WiFi.softAPSSID().c_str (), WiFi.channel());
    Serial.printf ("IP address: %s\n", WiFi.softAPIP().toString().c_str());
    Serial.printf ("MAC address: %s\n", WiFi.softAPmacAddress().c_str());
#ifdef ESP32
    quickEspNow.setWiFiBandwidth (WIFI_IF_AP, WIFI_BW_HT20); // Only needed for ESP32 in case you need coexistence with ESP8266 in the same network
#endif //ESP32
    quickEspNow.onDataRcvd (dataReceived);
    quickEspNow.begin (CURRENT_WIFI_CHANNEL, WIFI_IF_AP); // Same channel must be used for both AP and ESP-NOW
}

void loopEspNow() {
    static time_t lastSend = 60000;
    static unsigned int counter = 0;

    if (millis () - lastSend >= 10000) {//10seconds
        lastSend = millis ();
        String message = String (msg) + " " + String (counter++) +" @"+String(lastSend);
        if (!quickEspNow.send (DEST_ADDR, (uint8_t*)message.c_str (), message.length ())) {
            Serial.printf (">>>>>>>>>> Message sent\n");
        } else {
            Serial.printf (">>>>>>>>>> Message not sent\n");
        }

    }

}