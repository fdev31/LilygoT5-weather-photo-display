#include <Arduino.h>

void dumpDate(const char *prefix, struct tm d) {
#if ENABLE_SERIAL_DEBUG
    Serial.print(prefix);
    Serial.print(": ");
    Serial.print(d.tm_year + 1900);
    Serial.print("/");
    Serial.print(d.tm_mon + 1);
    Serial.print("/");
    Serial.println(d.tm_mday);
#endif
}