#include <Arduino.h>
#include "_main.h"

//#include <ESPmDNS.h>
#include <DNSServer.h>
#include <WiFi.h>
//#include <AsyncTCP.h>
//#include "ESPAsyncWebServer.h"
#include <esp_wifi.h>

bool _isSTA_GotIP=false;
bool _isAPconnected=false;
bool _isAPclosed=false;

uint32_t ipClientAddress;

void APclientIPassigned(WiFiEvent_t event, WiFiEventInfo_t info)
{   

    ipClientAddress = info.wifi_ap_staipassigned.ip.addr; 
    log_d("Client ip:%s", IPAddress(ipClientAddress).toString().c_str());

    _isAPconnected=true;
    if (_isPwmBuzzer) Play_135();
}
void APclientConnected(WiFiEvent_t event, WiFiEventInfo_t info)
{   

    ipClientAddress = info.wifi_ap_staipassigned.ip.addr; 
    log_w("AP client:%s", IPAddress(ipClientAddress).toString().c_str());
}
void APclientLeave(WiFiEvent_t event, WiFiEventInfo_t info)
{    
    log_w("AP client leave.");

    _isAPconnected=false;
    if (_isPwmBuzzer) Play_531();
}
/*
void APstarted(WiFiEvent_t event, WiFiEventInfo_t info)
{
  _isAPclosed=false;
  log_w("AP Started!");
}
void APclosed(WiFiEvent_t event, WiFiEventInfo_t info)
{
  _isAPclosed=true;
  log_w("AP Closed!");
}
void STAgotIP(WiFiEvent_t event, WiFiEventInfo_t info)
{
  _isSTA_GotIP=true;
  log_w("Got STA IP");
} 
*/
void setupWiFiEvents(){
  WiFi.onEvent(APclientIPassigned, WiFiEvent_t::ARDUINO_EVENT_WIFI_AP_STAIPASSIGNED);
  WiFi.onEvent(APclientConnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_AP_STACONNECTED);
  WiFi.onEvent(APclientLeave, WiFiEvent_t::ARDUINO_EVENT_WIFI_AP_STADISCONNECTED);
  //WiFi.onEvent(APstarted, WiFiEvent_t::ARDUINO_EVENT_WIFI_AP_START);
  //WiFi.onEvent(APclosed, WiFiEvent_t::ARDUINO_EVENT_WIFI_AP_STOP);
  //WiFi.onEvent(STAgotIP, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_GOT_IP); //STA_GOT_IP
}
