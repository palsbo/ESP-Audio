/*------------------------------------------------------
   Audio class for I2S and direct audio.
   For ESP8266 and ESP32
   With or without SPIRAM
   With or without I2S DAC
   by Erik Palsbo.
   Github: https://github.com/palsbo
   Using library from https://github.com/earlephilhower/ESP8266Audio
   ------------------------------------------------------

  SIGNAL              ESP8266    Wemos-D1 ESP32
  ------------------------------------------------------
  LRCK                GPIO2/TX1  D4       GPIO25      (Rød)
  DATA                GPIO3/RX0  RX0      GPIO22      (Orange)
  BCLK                GPIO15     D8       GPIO26      (Gul)


  23LC24   SPIRAM
  SIGNAL   23LC1024   ESP8266    Wemos-D1    ESP32
  ------------------------------------------------------
  CS          1       GPIO0      D3
  SOSIO1      2       GPIO12     D6
  SIO2        3       GND        GND
  VSS         4       GND        GND
  SISIO0      5       GPIO13     D7
  SCK         6       GPIO14     D5
  HOLDSIO3    7       3V3        3V3
  VCC         8       3V3        3V3
*/

#ifndef AUDIO_H
#define AUDIO_H

#ifndef VOLUME
#define VOLUME  40
#endif

#ifndef STREAMURL
#define STREAMURL "http://streammp3.retro-radio.dk/retro-mp3?type=.mp3"
#endif

#ifndef AUDIOBUFFERSIZE
#ifdef  ESP32
#define AUDIOBUFFERSIZE 26 * 1024
#endif
#ifdef ESP8266
#define AUDIOBUFFERSIZE 5 * 1024
#endif
#endif

#ifndef SPIBUFFERSIZE
#define SPIBUFFERSIZE 131072
#endif

#ifdef ICY
#include <AudioFileSourceICYStream.h>
#define AUDIOSOURCE AudioFileSourceICYStream
#else
#include <AudioFileSourceHTTPStream.h>
#define AUDIOSOURCE AudioFileSourceHTTPStream
#endif

#include <AudioGeneratorMP3.h>
#define AUDIOGENERATOR  AudioGeneratorMP3

#ifdef SPIRAM
#include <AudioFileSourceSPIRAMBuffer.h>
#define AUDIOBUFFER AudioFileSourceSPIRAMBuffer
#else
#include <AudioFileSourceBuffer.h>
#define AUDIOBUFFER AudioFileSourceBuffer
#endif

#ifdef DAC
#include <AudioOutputI2S.h>
#define AUDIOOUTPUT AudioOutputI2S
#else
#include <AudioOutputI2SNoDAC.h>
#define AUDIOOUTPUT AudioOutputI2SNoDAC
#endif

typedef void (*audiocallbacktype)(void *cbData, int code, const char *string);
audiocallbacktype cb = [] (void *cbData, int code, const char *string) {
  const char *ptr = reinterpret_cast<const char *>(cbData);
  // Note that the string may be in PROGMEM, so copy it to RAM for printf
  char s1[64];
  strncpy_P(s1, string, sizeof(s1));
  s1[sizeof(s1) - 1] = 0;
  Serial.printf("STATUS(%s) '%d' = '%s'\n", ptr, code, s1);
  Serial.flush();
};

class AUDIO {
  private:
#ifdef ERRORSTATUS
    static void StatusCallback(void *cbData, int code, const char *string);
#endif
    char buf[100];
    void  *preallocateBuffer = NULL;
    void  *preallocateCodec = NULL;
    int   retryms = 0;
    AUDIOGENERATOR *decoder;
    AUDIOSOURCE *file;
    AUDIOBUFFER *buff;
    AUDIOOUTPUT *out;
    void play();
    int testmem() {  //  to check how much ram is left
      void *buf = NULL;
      int i = 1;
      while (true) {
        buf = malloc(i++ * 1024);
        if (!buf) break;
        free(buf);
      }
      return i - 1;
    }
  public:
    int  volume = VOLUME;
    char URL[100] = STREAMURL;
    void begin();
    bool isPlaying();
    void setGain(int value);
    void stop();
    void start();
    void start(char * data);
    void loop();
    void onstatus(audiocallbacktype func);
};

/*
   OBS!
   Do not move belowe code to .cpp file
   as we depend on #define for SPIRRAM and DAC
*/

#ifdef ERRORSTATUS
void AUDIO::StatusCallback(void *cbData, int code, const char *string) {
  if (code == 257) return;
  cb(cbData, code, string);
}
#endif

void AUDIO::onstatus(audiocallbacktype func) {
  cb = func;
};
void AUDIO::begin() {
#ifndef SPIRAM
  sprintf(buf, "Buffersize: %d Bytes", AUDIOBUFFERSIZE);
  cb((void*)"malloc", 898, buf);
#endif
  file = NULL;
  buff = NULL;
  out = new AUDIOOUTPUT();
  out->SetGain(.1);
  //out->SetPinout(26,25,22);
  decoder = NULL;
  delay(1000);
}

void AUDIO::play() {
  file = new AUDIOSOURCE(URL);
#ifdef ERRORSTATUS
  file->RegisterStatusCB(StatusCallback, (void*)"file");
#endif
#ifdef SPIRAM
  buff = new AUDIOBUFFER(file, 0, SPIBUFFERSIZE);
#else
  buff = new AUDIOBUFFER(file, AUDIOBUFFERSIZE);
#endif
#ifdef ERRORSTATUS
  buff->RegisterStatusCB(StatusCallback, (void*)"buffer");
#endif
  decoder = new AUDIOGENERATOR();
  sprintf(buf, "Max free malloc is %d kB", testmem());
  cb((void*)"malloc", 898, buf);
#ifdef ERRORSTATUS
  decoder->RegisterStatusCB(StatusCallback, (void*)"decoder");
#endif
  out->SetGain(((float)volume) / 100.0);
  if (decoder->begin(buff, out)) {
    cb((void*)"Playing", 999, URL);
    retryms = 0;
  } else {
    cb((void*)"Stopped", 999, "Play error");
    stop();
    retryms = millis() + 1000;
  }
}

bool AUDIO::isPlaying() {
  if (!decoder) return false;
  if (!decoder->isRunning()) return false;
  return true;
}

void AUDIO::setGain(int value) {
  volume = value;
  out->SetGain(((float)volume) / 100.0);
}

void AUDIO::stop() {
  if (isPlaying()) decoder->stop();
  if (decoder) delete decoder, decoder = NULL;
  if (buff) buff->close(), delete buff, buff = NULL;
  if (file) file->close(), delete file, file = NULL;
}

void AUDIO::start() {
  stop();
  retryms = millis() + 1000;
}

void AUDIO::start(char * data) {
  strcpy(URL, data);
  start();
}

void AUDIO::loop()  {
  if (decoder && decoder->isRunning()) {
    if (!decoder->loop()) {
      stop();
      retryms = millis() + 1000;
    }
  } else  {
    if ((millis() > retryms) && retryms) play();
  }
}


#endif


