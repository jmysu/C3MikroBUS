#include <Arduino.h>
#include <WiFi.h>
#include <WiFiAP.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <Preferences.h>
#include <esp_ota_ops.h>
/* INCLUDE ESP2SOTA LIBRARY */
#include <ESP2SOTA.h>
#include "_main.h"

bool _isOTA=false;
Preferences myPref;
const char* ssid = "C3mBUS-FactoryOTA";
const char* passwordAP = "ota0806449";
WebServer server(80);
const byte DNS_PORT = 53;
DNSServer dnsServer;

const char htmlCaptivePortal[] PROGMEM = R"=====(
  <!DOCTYPE html> <html>
    <head>
      <title>C3mBUS Captive Portal</title>
      <style>
        body {background-color:#06cc13;}
        h1 {color: white;}
        h2 {color: white;}
      </style>
      <meta name="viewport" content="width=device-width, initial-scale=1.0">
    </head>
    <body>
      <h1>Hello World!</h1>
      <h2>Click this <a href="/update">link</a> to start upgrade...
 </h2>
    </body>
  </html>
)=====";

// This will set the otadata to boot from the factory image, ignoring previous OTA updates.     *
//***********************************************************************************************
/*
void backtoFactory(){
  esp_partition_iterator_t  pi ;                                  // Iterator for find
  const esp_partition_t*    factory ;                             // Factory partition  
  esp_err_t                 err ;

  pi = esp_partition_find ( ESP_PARTITION_TYPE_APP,               // Get partition iterator for
                            ESP_PARTITION_SUBTYPE_APP_FACTORY,    // Factory partition
                            "factory" ) ;
  if ( pi == NULL ) {                                             // Check result
        log_e ( "Failed to find factory partition" ) ;
      }
  else {
        factory = esp_partition_get ( pi ) ;                         // Get partition struct
        esp_partition_iterator_release ( pi ) ;                      // Release the iterator
        err = esp_ota_set_boot_partition ( factory ) ;               // Set partition for boot
        if ( err != ESP_OK ) {                                       // Check error
          log_e ( "Failed to set boot partition" ) ;
          }
        else {
          log_w("Back to factory!");
          delay(1000);
          esp_restart() ;                                         // Restart ESP
          }
      }
}
*/
void backtoApp(){
  esp_partition_iterator_t  pi ;                                  // Iterator for find
  const esp_partition_t*    app ;                                 // App partition  
  esp_err_t                 err ;

  pi = esp_partition_find ( ESP_PARTITION_TYPE_APP,               // Get partition iterator for
                            ESP_PARTITION_SUBTYPE_APP_OTA_0,      // SubType OTA_0 partition
                            "app" ) ;                             // Patition name "app"  
  if ( pi == NULL ) {                                             // Check result
        log_e ( "Failed to find 'app' partition" ) ;
      }
  else {
        app = esp_partition_get ( pi ) ;                         // Get partition struct
        esp_partition_iterator_release ( pi ) ;                  // Release the iterator
        err = esp_ota_set_boot_partition ( app ) ;               // Set partition for boot
        if ( err != ESP_OK ) {                                   // Check error
          log_e ( "Failed to set boot partition 'app'" ) ;
          }
        else {
          log_w("Boot to APP!");
          delay(1000);
          esp_restart() ;                                         // Restart ESP
          }
      }
}

void setup(void) {
  //Serial.begin(115200);
  #if (ARDUINO_USB_CDC_ON_BOOT)
    log_w("USB_CDC_ON_BOOT enabled!");
    if (isUSBCDC()) {
      if (!Serial) {Serial.begin(115200); delay(300);}
      }
  #else
   log_w("USB_CDC_ON_BOOT disabled!");
   if (!Serial) {Serial.begin(115200); delay(300);}       
  #endif

  printESP32info();

  neopixelRGB(8,0,0); //Red
  neopixelSetupBlink(500);//500ms blink
  
  //Mount SD, Check SD update.bin, go SD-Updater if available!


  //NVS Udpate
  myPref.begin("my-app", false); //ReadWrite mode
  _isOTA=myPref.getBool("OTA", false);
  log_w("NVS OTA:%s", (_isOTA?"Yes":"No"));
  myPref.putBool("OTA", false); //reset OTA to false
  myPref.end();
  
  //Normal No SD Update here..., Start OTA CaptivePortal by default
  if (1) {
    //Get 32bit chip id from reversed MAC address
    uint32_t ChipID32 = getESP32ChipID24();
    char cAP[32];
    sprintf(cAP, "C3FactoryOTA-%04X", (uint16_t)ChipID32);    //for 4 nibbles 

    setupWiFiEvents();
    WiFi.mode(WIFI_AP);  
    //WiFi.softAP(ssid, NULL); //Open HotSpot with ssid
    //WiFi.softAP(cAP, NULL); //Open HotSpot with cAP+ID
    WiFi.softAP(cAP, passwordAP);  
    delay(100);
    IPAddress IP = IPAddress (8, 8, 4, 4);
    IPAddress NMask = IPAddress (255, 255, 255, 0);
    WiFi.softAPConfig(IP, IP, NMask);
    IPAddress myIP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(myIP);

    // if DNSServer is started with "*" for domain name, it will reply with provided IP to all DNS request
    dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
    dnsServer.start(DNS_PORT, "*", IP);

    // replay to all requests with same HTML
    server.onNotFound([]() {
      server.send(200, "text/html", htmlCaptivePortal);
    });

    /* INITIALIZE ESP2SOTA LIBRARY */
    ESP2SOTA.begin(&server);
    server.begin();
    Play_RampUp();

    setupButtonBoot();
    #if defined(BOARDNAME)
      log_w("C3FactoryOTA on %s started! Long press BootButton to boot into APP!!!", String(BOARDNAME).c_str());
    #endif
    //setupEspNow();

    }
  //else {//No OTA  
  //  Play_RampDown();
  //  setupButtonBoot();
  //  #if defined(BOARDNAME)
  //    log_w("NO OTA on %s started! Wait BootButton!", String(BOARDNAME).c_str());
  //  #endif
  //  }
}

void loop(void) {
  static bool isAPconnected=false;  

  if (1) { 
    /* HANDLE DNS */
    dnsServer.processNextRequest();
    /* HANDLE UPDATE REQUESTS */
    server.handleClient();

    if (isAPconnected!=_isAPconnected){
      isAPconnected=_isAPconnected;
      if (isAPconnected) {
        neopixelRGB(0,8,0); //Green
        neopixelSetupBlink(500);//500ms blink
        }
      else {
        neopixelRGB(0,0,8); //Blue
        neopixelSetupBlink(500);//2000ms blink
        }   
      }

    //loopEspNow();  
    loopButtonBoot();
    }
}