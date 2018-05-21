#define DAC   //  uncomment if I2S DAC module is  used
#define ERRORSTATUS
#define ICY

#ifdef  ESP32
#define AUDIOBUFFERSIZE 26 * 1024
#endif
#ifdef ESP8266
#define AUDIOBUFFERSIZE 5 * 1024 + 512
#endif

#ifdef ESP8266
/*  OBS! OBS! OBS!
  Remember to set paramters in Arduino IDE
	Tools->lwIP Variant->v1.4 Open Source, or V2 Higher Bandwidth
	Tools->CPU Frequency->160MHz
*/
// #define SPIRAM  //  uncomment if 23LC1024 SPI RAM is used
#include <ESP8266WiFi.h>
#else
#include <WiFi.h>
#endif
#include "audio.h"
#include "sock.h"
#include "myntp.h"

#define SSID  "..";
#define PASS  "..";
#define WSHOST  "..";
#define WSPORT  81;

#include <init.h> //  I use this library file to redefine the configuration values - remove if config data are defined abowe
const char *ssid = SSID;
const char *password = PASS;
const char *wshost = WSHOST;
int wsport = WSPORT;

AUDIO radio;
SOCK sock;

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  myntp.begin();
  sock.on("connect", [](char* data) {
    Serial.printf("[WSc] Connected! %s\n", data);
  });
  sock.on("disconnect", [](char* data) {
    Serial.printf("[WSc] Disconnected!%s\n", data);
  });
  sock.on("url", [](char* data) {
    Serial.printf("[WSc] URL! '%s'\n", data);
    radio.start(data);
  });
  sock.on("gain", [](char* data) {
    Serial.printf("[WSc] Volume! '%s'\n", data);
    radio.setGain(atoi(data));
  });
  sock.begin(wshost, wsport);
  radio.onstatus([](void *cbData, int code, const char *string) {
    const char *ptr = reinterpret_cast<const char *>(cbData);
    // Note that the string may be in PROGMEM, so copy it to RAM for printf
    char s1[64];
    strncpy_P(s1, string, sizeof(s1));
    s1[sizeof(s1) - 1] = 0;
    Serial.print(myntp.timeNow());
    Serial.printf(" STATUS(%s) '%d' = '%s'\n", ptr, code, s1);
    Serial.flush();
  });
  radio.begin();
}

void loop() {
  radio.loop();
  sock.loop();
}
