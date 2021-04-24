#include <Arduino.h>
#include <WiFi.h>
// esp
#include "esp_himem.h"
#include "esp_spiram.h"
// battery
#include <driver/adc.h>
#include "esp_adc_cal.h"
// screen
#include "epd_driver.h"
#include "epd_highlevel.h"
// project's
#include "display.h"
#include "garbage.h"
#include "wttr.h" // weather info, check wttr.in
#include "configserver.h"
#include "configuration.h"
#include "background.h"


uint64_t WAKEUP_INTERVAL = (uint64_t) 1000*1000*3600*INTERVAL;
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 3600;
int settings_mode = false;
static struct tm timeinfo; // today's date
extern int temperature;
int isMaintenanceWakup = false;
int vref = 1100;
Configuration config;
#define BATT_PIN            36
#define BATTERY_POS 765

EpdRect fullscreenArea = {
    .x = 0,
    .y = 0,
    .width = EPD_WIDTH,
    .height = EPD_HEIGHT,
};

/**
 * Correct the ADC reference voltage. Was in example of lilygo epd47 repository to calc battery percentage
 * (DavidM42)
*/
void correct_adc_reference()
{
    esp_adc_cal_characteristics_t adc_chars;
    esp_adc_cal_value_t val_type = esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 1100, &adc_chars);
    if (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF) {
        Serial.printf("eFuse Vref:%u mV", adc_chars.vref);
        vref = adc_chars.vref;
    }
}

void basic_init() {
  Serial.begin(115200);
  if (psramInit()) {
    Serial.println("\nThe PSRAM is correctly initialized");
  } else {
    Serial.println("\nPSRAM does not work");
  }
  isMaintenanceWakup = esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_TIMER;
  // WiFi setup
  WiFi.mode(WIFI_STA);

  // Read SSID & Password from EEPROM
  config = init_config();
  if (config.valid) {
    Serial.println("Connecting to wifi");
    WiFi.begin(config.ssid, config.pass);
  }
}

void display_init() {
  Serial.println("Open display");
  epd_init(EPD_OPTIONS_DEFAULT);
  hl = epd_hl_init(EPD_BUILTIN_WAVEFORM);
  fb = epd_hl_get_framebuffer(&hl);
}

void time_init() {
  Serial.println("Sync time");
  // NTP Setup
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  while (!getLocalTime(&timeinfo)) {
    Serial.println("Error getting time, restarting");
    esp_restart();
  }
}

double get_battery_level()
{
  uint16_t v = analogRead(BATT_PIN);
  return ((float)v / 4095.0) * 2.0 * 3.3 * (vref / 1000.0);
}

void show_battery_level()
{
  Serial.println("Show battery");
  double level = get_battery_level();
  int w = (level - 3.0)*47.5; // 57 pixels width, 1.2 == 100%
  if (w > 57) w = 57;
  else if (w < 0) w = 0;
  char text[50];
  sprintf(text, "%.2fV", level);
  int x = EPD_WIDTH - 100;
  int y = 40;
  epd_write_string(&stdfont, text, &x, &y, fb, &rightAlignedStyle);
  if (w) {
    EpdRect battery_rect = {.x = 870, .y = 17, .width = w, .height = 15};
    epd_fill_rect(battery_rect, 1, fb);
  }
}

void show_garbage() {
  Serial.println("Show garbage pickup");
  int x, y;
  GarbageDays gd = check_garbage(timeinfo);
  //    drawIcon(EPD_WIDTH - SMALL_ICON, EPD_HEIGHT - SMALL_ICON, (const char **)img, SMALL_ICON, SMALL_ICON);

  x = 912;
  y = 456;
  epd_write_string(&stdfont, gd.paper, &x, &y, fb, &leftAlignedStyle);

  x = 912;
  y = 505;
  epd_write_string(&stdfont, gd.std, &x, &y, fb, &leftAlignedStyle);
  free_garbage(&gd);
}

void drawWeatherChart(int x, int y, int initVal, int increment, T_Snap previsions[], int (*getFunc)(int, T_Snap[])) {
  int i=0;
  int prevVal = y-initVal;
  int thickness = 1;

  epd_draw_line(x, prevVal, x + FORECAST_NB_DAYS*FORECAST_DAILY_SNAPS*increment, prevVal, 127, fb);
  for (int daycount=0; daycount<FORECAST_NB_DAYS; daycount++) {
    for (int snapcount=0; snapcount<FORECAST_DAILY_SNAPS; snapcount++) {
      int val = getFunc(i, previsions);
      val = y-val;
      epd_draw_line(x, prevVal -1, x + increment, val -1, 15, fb);
      for (int i=0; i<thickness; i++)
        epd_draw_line(x, prevVal+i, x + increment, val+i, 0, fb);
      prevVal = val;
      x+=increment;
      i++;
    }
  }
}

void show_weather() {
  Serial.println("Show weather");
  WeatherSnapshot weather = getWeather();
  if(!weather.title) return;
  Serial.println("Received weather data");
  int x = BATTERY_POS/2, y = 70;
  char text[50];

  epd_write_string(&titlefont, weather.title, &x, &y, fb, &centeredStyle);

  x=6;
  y = EPD_HEIGHT-6;
  EpdFontProperties dateStyle = { WHITE, 8, '?', EPD_DRAW_ALIGN_LEFT};
  epd_write_string(&stdfont, weather.observationDate, &x, &y, fb, &dateStyle);
  y = EPD_HEIGHT-6;
  x += 100;
  epd_write_string(&stdfont, weather.moonphase, &x, &y, fb, &dateStyle);

  int topMargin = 120;
  int leftMargin = 65;

  int segmentSize=10;
  int chartSpacing = 60;
  int chartLeftMargin = 150;

  // background
  x = chartLeftMargin; y = topMargin;
  for (int daycount=1; daycount<FORECAST_NB_DAYS; daycount++) {
    epd_draw_line(x, y, x, y+(chartSpacing*3), 50, fb);
    x+= FORECAST_DAILY_SNAPS*segmentSize;
  }
  // 1st
  x = leftMargin;
  weather.now.getTemperature((char *)&text);
  epd_write_string(&stdfont, text, &x, &y, fb, &rightAlignedStyle);
  drawWeatherChart(chartLeftMargin, topMargin,
            weather.now.temperature*2,
            segmentSize, weather.previsions,
            [](int i, T_Snap previsions[]) {return previsions[i].temperature*2;}
            );
  // 2nd
  x = leftMargin; y = topMargin + chartSpacing;
  weather.now.getWind((char *)&text);
  epd_write_string(&stdfont, text, &x, &y, fb, &rightAlignedStyle);
  drawWeatherChart(chartLeftMargin, topMargin + chartSpacing,
            weather.now.wind,
            segmentSize, weather.previsions,
            [](int i, T_Snap previsions[]) {return previsions[i].wind;}
            );
  // 3rd
  x = leftMargin; y = topMargin + (chartSpacing*2);
  weather.now.getCloud((char *)&text);
  epd_write_string(&stdfont, text, &x, &y, fb, &rightAlignedStyle);
  drawWeatherChart(chartLeftMargin, topMargin + (chartSpacing*2),
            weather.now.cloudcover/2,
            segmentSize, weather.previsions,
            [](int i, T_Snap previsions[]) {return previsions[i].cloudcover/2;}
            );
  // 4th
  x = leftMargin; y = topMargin + (chartSpacing*3);
  weather.now.getPrecipitation((char *)&text);
  epd_write_string(&stdfont, text, &x, &y, fb, &rightAlignedStyle);
  drawWeatherChart(chartLeftMargin, topMargin + (chartSpacing*3),
            weather.now.precipitation*10,
            segmentSize, weather.previsions,
            [](int i, T_Snap previsions[]) {return int(previsions[i].precipitation*10.0);}
            );
  /*
  // 5th
  x = leftMargin; y = topMargin + (chartSpacing*4);
  weather.now.getHumidity((char *)&text);
  epd_write_string(&stdfont, text, &x, &y, fb, &rightAlignedStyle);
  drawWeatherChart(chartLeftMargin, topMargin + (chartSpacing*4),
            weather.now.humidity/2,
            segmentSize, weather.previsions,
            [](int i, T_Snap previsions[]) {return previsions[i].humidity/2;}
            );
            */
  cleanupWeatherData(weather);
}

void setup()
{
  basic_init();
  display_init();
  epd_poweron();
  if (!config.valid || WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    Serial.println("Invalid configuration or no connexion, running settings...");
    epd_fullclear(&hl, temperature);
    start_settings_mode();
    settings_mode = true;
  } else {
    delay(50);
    time_init();
    epd_fullclear(&hl, temperature);
    epd_copy_to_framebuffer(fullscreenArea, background_data, epd_hl_get_framebuffer(&hl));
    show_weather();
    correct_adc_reference();
    show_battery_level();
    show_garbage();
    epd_hl_update_screen(&hl, MODE_GC16, temperature);
  }
}

void loop()
{
  if (settings_mode) {
    handle_configserver();
  } else {
    WiFi.disconnect();
    WiFi.mode(WIFI_OFF);
    epd_poweroff();
    esp_sleep_enable_timer_wakeup(WAKEUP_INTERVAL);
    esp_deep_sleep_start();
  }
}
