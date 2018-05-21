/*------------------------------------------------------
 * Webradio with simple WebServer
 * For ESP8266 and ESP32 (ESP8266 requires SPIRAM)
 * With or without SPIRAM
 * With or without I2S DAC
 * by Erik Palsbo.
 * Github: https://github.com/palsbo
 * Using library from https://github.com/earlephilhower/ESP8266Audio
 * ------------------------------------------------------
 */
#define DAC   //  uncomment if I2S DAC module is  used
#define ERRORSTATUS
#define ICY

#ifdef ESP8266
/*
Remember to set paramters in Arduino IDE
	Tools->lwIP Variant->v1.4 Open Source, or V2 Higher Bandwidth
	Tools->CPU Frequency->160MHz
*/
//#define SPIRAM  //  uncomment if 23LC1024 SPI RAM is used
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#else
#include <WiFi.h>
#include <ESP32WebServer.h>
#endif
#include <audio.h>
#include "pages.h"
#include "myntp.h"

#include <init.h>   //  OBS! remove this line and insert your own SSID and PASS
const char* ssid = SSID;
const char* password = PASS;

#ifdef  ESP8266
ESP8266WebServer server(80);
#else
ESP32WebServer server(80);
#endif
AUDIO radio;

void checkargs() {
  String url = server.arg("url");
  int volume = atoi(server.arg("volume").c_str());
  if (server.arg("url")!="") radio.start(&url[0]);
  if (server.arg("volume")!="") radio.setGain(volume);
}

void handleRoot() {
  radio.stop();
  delay(1000);
  checkargs();
  String s = index_html;
  s.replace("%%URL%%", radio.URL);
  s.replace("%%VOL%%", String(radio.volume));
  server.send(200, "text/html", s);
  delay(1000);
  radio.start();
}

void handleNotFound(){
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}
void setup(void){
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
  server.on("/", handleRoot);
  server.on("/set", checkargs);
  server.onNotFound(handleNotFound);
  server.begin();
  radio.begin();
  Serial.println("HTTP server started");
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
  radio.start("http://streammp3.retro-radio.dk/retro-mp3?type=.mp3");
}

void loop(void){
  server.handleClient();
  radio.loop();
}