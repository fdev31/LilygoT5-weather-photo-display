#ifndef _WTTR_H
#define _WTTR_H
#include <stdio.h>
// 3 days, 7 snapshots per day
#define FORECAST_SIZE 21
#define FORECAST_NB_DAYS 3
#define FORECAST_DAILY_SNAPS 7
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
    void getPrecipitation(char *text) {
        sprintf(text, "%.1f", this->precipitation);
    };
    int temperature;
    int humidity;
    int cloudcover;
    int wind;
    double precipitation;
};

typedef struct {
    char *title;
    char *moonphase;
    char *observationDate;
    T_Snap now;
    T_Snap previsions[FORECAST_SIZE];
} WeatherSnapshot;

WeatherSnapshot getWeather();
void cleanupWeatherData(WeatherSnapshot);
#endif
