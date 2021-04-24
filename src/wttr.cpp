#include <Arduino.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <iostream>
#include "esp_himem.h"
#include "esp_spiram.h"

#include "wttr.h"

#define DOC_SIZE 60000

struct SpiRamAllocator {
  void* allocate(size_t size) {
    return ps_malloc(size);
  }
  void deallocate(void* pointer) {
    free(pointer);
  }
  void* reallocate(void* ptr, size_t new_size) {
    return ps_realloc(ptr, new_size);
  }
};

using SpiRamJsonDocument = BasicJsonDocument<SpiRamAllocator>;

void cleanupWeatherData(WeatherSnapshot snap) {
  if (snap.title) free(snap.title);
  if (snap.observationDate) (snap.observationDate);
  if (snap.moonphase) (snap.moonphase);
}

WeatherSnapshot getWeather() {
    SpiRamJsonDocument doc(DOC_SIZE);
    WeatherSnapshot ret;
    HTTPClient http;

    http.begin("http://wttr.in/?format=j1");
    int httpCode = http.GET();
    if (httpCode == 200)
    {
      WiFiClient cli = http.getStream();

      deserializeJson(doc, cli);
      ret.title = strdup(doc["current_condition"][0]["weatherDesc"][0]["value"].as<String>().c_str());
      ret.moonphase = strdup(doc["weather"][0]["astronomy"][0]["moon_phase"].as<String>().c_str());
      ret.observationDate = strdup(doc["current_condition"][0]["localObsDateTime"].as<String>().c_str());

      ret.now.temperature = atoi(doc["current_condition"][0]["temp_C"].as<String>().c_str());
      ret.now.wind = atoi(doc["current_condition"][0]["windspeedKmph"].as<String>().c_str());
      ret.now.cloudcover = atoi(doc["current_condition"][0]["cloudcover"].as<String>().c_str());
      ret.now.precipitation = atoi(doc["current_condition"][0]["precipMM"].as<String>().c_str());
//      ret.now.humidity = atoi(doc["current_condition"][0]["humidity"].as<String>().c_str());

      int i=0;
      for (int nbday=0; nbday<FORECAST_NB_DAYS; nbday++) {
        JsonArray entry = doc["weather"][nbday]["hourly"];
        for (int houridx = 0; houridx < FORECAST_DAILY_SNAPS; houridx++)
        {
          ret.previsions[i].temperature = atoi(entry[houridx]["tempC"].as<String>().c_str());
          ret.previsions[i].wind = atoi(entry[houridx]["WindGustKmph"].as<String>().c_str());
          ret.previsions[i].cloudcover = atoi(entry[houridx]["cloudcover"].as<String>().c_str());
          ret.previsions[i].precipitation = atof(entry[houridx]["precipMM"].as<String>().c_str());
//          ret.previsions[i].humidity = atoi(entry[houridx]["humidity"].as<String>().c_str());
          i++;
        }
      }
    } else {
      Serial.println("Error getting weather data !!");
      ret.title = NULL;
    }
    return ret;
}
