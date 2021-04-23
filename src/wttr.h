#ifndef _WTTR_H
#define _WTTR_H
#include <stdio.h>
// 3 days, 7 snapshots per day
#define FORECAST_SIZE 21
class T_Snap {
public:
    void getTemperature(char *text) {
        sprintf(text, "%d", this->temperature);
    };
    void getHumidity(char *text) {
        sprintf(text, "%d", this->humidity);
    };
    void getCloud(char *text) {
        sprintf(text, "%d", this->cloudcover);
    };
    void getWind(char *text) {
        sprintf(text, "%d", this->wind);
    };
    int temperature;
    int humidity;
    int cloudcover;
    int wind;
};

typedef struct {
    char *title;
    T_Snap now;
    T_Snap previsions[FORECAST_SIZE];
} WeatherSnapshot;

WeatherSnapshot getWeather();

#endif
