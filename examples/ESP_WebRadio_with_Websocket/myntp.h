#ifndef MYNTP_H
#define MYNTP_H

#include <TimeLib.h>
#ifdef ESP8266
#include <simpleDSTadjust.h>
#endif

#define NTP_SERVERS "us.pool.ntp.org", "pool.ntp.org", "time.nist.gov"
#define timezone +1 // 
#ifdef ESP8266
struct dstRule StartRule = {"CET", Last, Sun, Mar, 2, 3600};    // Daylight time = UTC/GMT -4 hours
struct dstRule EndRule = {"CET", Last, Sun, Oct, 2, 0};       // Standard time = UTC/GMT -5 hour
simpleDSTadjust dstAdjusted(StartRule, EndRule);
#endif

class MYNTP {
  private:
  public:
    void adjust() {
#ifdef ESP8266
      setTime(dstAdjusted.time(nullptr));
#else
      struct tm timeinfo;
      if (getLocalTime(&timeinfo)) {
        setTime(timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec, timeinfo.tm_mday, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900);
      }
#endif
    }
    void begin() {
#ifdef ESP8266
      configTime(timezone * 3600, 0, NTP_SERVERS);
      while (time(nullptr) < 100000) delay(200);
#else
      configTime(0, 0, "pool.ntp.org");
      while (time(nullptr) < 100000) delay(200);
      setenv("TZ", "GMT-1GMT-2,M3.4.0/01,M10.4.0/02", 1);
#endif
      adjust();
    }
    String twoDigits(int digits) {
      if (digits > 9) return String(digits);
      return '0' + String(digits);
    }

    String timeNow() {
      adjust();
      return twoDigits(hour()) + ":" + twoDigits(minute()) + ":" + twoDigits(second());
    }

    String dateNow() {
      adjust();
      return twoDigits(day()) + "-" + twoDigits(month()) + "-" + twoDigits(year());
    }
} myntp;

#endif

