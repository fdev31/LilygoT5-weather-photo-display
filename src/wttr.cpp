#include <Arduino.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <iostream>
#include "esp_himem.h"
#include "esp_spiram.h"

#include "wttr.h"

#define MAX_DOC_SIZE 60000

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

WeatherSnapshot getWeather() {
    SpiRamJsonDocument doc(MAX_DOC_SIZE);
// char *buffer = (char*) ps_malloc(MAX_DOC_SIZE);
    WeatherSnapshot ret;
    HTTPClient http;

    http.begin("http://wttr.in/?format=j1");
    int httpCode = http.GET();
    if (httpCode == 200)
    {
      WiFiClient cli = http.getStream();
//      cli.read((uint8_t *)buffer, MAX_DOC_SIZE);
      //buffer[MAX_DOC_SIZE - 1] = 0;

      deserializeJson(doc, cli);
      ret.title = strdup(doc["current_condition"][0]["weatherDesc"][0]["value"].as<String>().c_str());

      ret.now.temperature = atoi(doc["current_condition"][0]["temp_C"].as<String>().c_str());
      ret.now.wind = atoi(doc["current_condition"][0]["windspeedKmph"].as<String>().c_str());
      ret.now.cloudcover = atoi(doc["current_condition"][0]["cloudcover"].as<String>().c_str());
      ret.now.humidity = atoi(doc["current_condition"][0]["humidity"].as<String>().c_str());

      Serial.println("~~~~~~~~~~~~~~~~~~~~~");
      int i=0;
      for (int nbday=0; nbday<3; nbday++) {
        JsonArray entry = doc["weather"][nbday]["hourly"];
        for (int houridx = 0; houridx < 7; houridx++)
        {
          /*
          Serial.print(i);
          Serial.print("=");
          Serial.print(nbday);
          Serial.print(",");
          Serial.print(houridx);
          Serial.print("=");
          Serial.println(entry[houridx]["FeelsLikeC"].as<String>());
          */
          
          ret.previsions[i].temperature = atoi(entry[houridx]["tempC"].as<String>().c_str());
          ret.previsions[i].wind = atoi(entry[houridx]["WindGustKmph"].as<String>().c_str());
          ret.previsions[i].cloudcover = atoi(entry[houridx]["cloudcover"].as<String>().c_str());
          ret.previsions[i].humidity = atoi(entry[houridx]["humidity"].as<String>().c_str());
//          Serial.println(ret.previsions[i].temperature);
          /*
          ret.previsions[i].wind = atoi(entry["WindGustKmph"].as<String>().c_str());
          ret.previsions[i].cloudcover = atoi(entry["cloudcover"].as<String>().c_str());
          ret.previsions[i].humidity = atoi(entry["humidity"].as<String>().c_str());
          */
          i++;
        }
      }
    }
    return ret;
}