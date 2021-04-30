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

// icons
#ifdef ENABLE_BATTERY_IMAGE
#include "images/battery.h"
#endif
#ifdef ENABLE_GARBAGE
#include "images/paper_icon.h"
#include "images/recycle_icon.h"
#endif

#ifdef ENABLE_WEATHER
#include "images/temp_icon.h"
#include "images/wind_icon.h"
#include "images/cloud_icon.h"
#include "images/rain_icon.h"
#endif
//#include "images/humid_icon.h"

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

#ifdef ENABLE_BACKGROUND
#define NB_BACKGROUNDS sizeof(backgrounds)/sizeof(backgrounds[0])
#endif

#ifdef ENABLE_MOON_PHASE_IMAGE
#include "images/moon_first_q.h"
#include "images/moon_full.h"
#include "images/moon_last_q.h"
#include "images/moon_new.h"
#include "images/moon_wan_cre.h"
#include "images/moon_wan_gib.h"
#include "images/moon_wax_cre.h"
#include "images/moon_wax_gib.h"

const uint8_t *moon_phases[] = {
  moon_new_data,
  moon_wax_cre_data,
  moon_first_q_data,
  moon_wax_gib_data,
  moon_full_data,
  moon_wan_gib_data,
  moon_last_q_data,
  moon_wan_cre_data
};
#endif

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
        vref = adc_chars.vref;
    }
}

void basic_init() {
  psramInit();
  isMaintenanceWakup = esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_TIMER;
  // WiFi setup
  WiFi.mode(WIFI_STA);

  // Read SSID & Password from EEPROM
  config = init_config();
  if (config.valid) {
    WiFi.begin(config.ssid, config.pass);
  }
}

void display_init() {
  epd_init(EPD_OPTIONS_DEFAULT);
  hl = epd_hl_init(EPD_BUILTIN_WAVEFORM);
  fb = epd_hl_get_framebuffer(&hl);
}

void time_init() {
  // NTP Setup
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  while (!getLocalTime(&timeinfo)) {
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
  double level = get_battery_level();
  int w = (level - 3.0)*47.5; // 57 pixels width, 1.2 == 100%
  if (w > 57) w = 57;
  else if (w < 0) w = 0;
  char text[50];
  sprintf(text, "%.2fV", level);
  int x = BATTERY_X_POSITION;
  int y = 35 + BATTERY_Y_POSITION;

#ifdef ENABLE_BATTERY_IMAGE
  drawImage(x+5, BATTERY_Y_POSITION, battery_width, battery_height, battery_data);
  if (w) {
    EpdRect battery_rect = {.x = BATTERY_X_POSITION+10, .y = 5+BATTERY_Y_POSITION, .width = w, .height = 30 };
    epd_fill_rect(battery_rect, 1, fb);
  }
#endif
#ifdef ENABLE_BATTERY_TEXT
  write_string_with_outline(&stdfont, text, &x, &y, &rightAlignedStyle);
#endif
}

#ifdef ENABLE_GARBAGE
void show_garbage() {
  int x, y;
  GarbageDays gd = check_garbage(timeinfo);
  //    drawIcon(EPD_WIDTH - SMALL_ICON, EPD_HEIGHT - SMALL_ICON, (const char **)img, SMALL_ICON, SMALL_ICON);

  x = GARBAGE_X_POSITION;
  y = GARBAGE_Y_POSITION;
  drawImage(x-paper_icon_width, y-(paper_icon_height*0.5)-10, paper_icon_width, paper_icon_height, paper_icon_data);
  write_string_with_outline(&stdfont, gd.paper, &x, &y, &leftAlignedStyle);

  x = GARBAGE_X_POSITION;
  y = GARBAGE_Y_POSITION + paper_icon_height;
  drawImage(x-recycle_icon_width, y-(recycle_icon_height*0.5)-10, recycle_icon_width, recycle_icon_height, recycle_icon_data);
  write_string_with_outline(&stdfont, gd.std, &x, &y, &leftAlignedStyle);
  free_garbage(&gd);
}
#endif

#ifdef ENABLE_WEATHER
void drawWeatherChart(int x, int y, int initVal, int increment, T_Snap previsions[], int (*getFunc)(int, T_Snap[])) {
  int i=0;
  int prevVal = y-initVal;
  int thickness = 2;

  epd_draw_line(x, prevVal, x + FORECAST_NB_DAYS*FORECAST_DAILY_SNAPS*increment, prevVal, 127, fb);
  for (int daycount=0; daycount<FORECAST_NB_DAYS; daycount++) {
    for (int snapcount=0; snapcount<FORECAST_DAILY_SNAPS; snapcount++) {
      int val = getFunc(i, previsions);
      val = y-val;
      epd_draw_line(x, prevVal - 1, x + increment, val -1, 255, fb);
      epd_draw_line(x, prevVal + thickness, x + increment, val + thickness, 255, fb);
      for (int i=0; i<thickness; i++)
        epd_draw_line(x, prevVal+i, x + increment, val+i, 0, fb);
      prevVal = val;
      x+=increment;
      i++;
    }
  }
}

void show_weather() {
  WeatherSnapshot weather = getWeather();
  if(!weather.title) return;
  int x = BATTERY_POS/2, y = 70;
  char text[50];

  write_string_with_outline(&titlefont, weather.title, &x, &y, &centeredStyle);

  x=6;
  y = EPD_HEIGHT-MOON_PHASES_BOTTOM_MARGIN;
  EpdFontProperties dateStyle = { WHITE, 8, '?', EPD_DRAW_ALIGN_LEFT};
  write_string_with_outline(&stdfont, weather.observationDate, &x, &y, &dateStyle);
  y = EPD_HEIGHT-MOON_PHASES_BOTTOM_MARGIN;
  x += MOON_PHASES_LEFT_MARGIN;
  #ifdef ENABLE_MOON_PHASE_TEXT
  write_string_with_outline(&stdfont, weather.moonphase, &x, &y, &dateStyle);
  #endif
  #ifdef ENABLE_MOON_PHASE_IMAGE
  drawImage(WEATHER_MOON_X_POS, WEATHER_MOON_Y_POS, moon_new_width, moon_new_height, moon_phases[weather.moonphaseIndex]);
  #endif

  int topMargin = WEATHER_TOP_MARGIN;
  int leftMargin = WEATHER_LEFT_MARGIN;

  int segmentSize = WEATHER_CHART_SEGMENT_SIZE;
  int chartSpacing = WEATHER_CHART_Y_SPACING;
  int chartLeftMargin = WEATHER_CHART_LEFT_MARGIN;
  int iconSpacing = chartSpacing/2;


  // background
  x = chartLeftMargin; y = topMargin;
  for (int daycount=1; daycount<FORECAST_NB_DAYS; daycount++) {
    epd_draw_line(x, y, x, y+(chartSpacing*3), 50, fb);
    x+= FORECAST_DAILY_SNAPS*segmentSize;
  }
  // 1st
  x = leftMargin;
  weather.now.getTemperature((char *)&text);
  drawImage(x+WEATHER_CHART_ICON_LEFT_MARGIN, y-iconSpacing, temp_icon_width, temp_icon_height, temp_icon_data);
  y += WEATHER_CHART_VALUE_Y_OFFSET;
  write_string_with_outline(&stdfont, text, &x, &y, &rightAlignedStyle);
  drawWeatherChart(chartLeftMargin, topMargin,
            weather.now.temperature*2,
            segmentSize, weather.previsions,
            [](int i, T_Snap previsions[]) {return previsions[i].temperature*2;}
            );
  // 2nd
  x = leftMargin; y = topMargin + chartSpacing;
  weather.now.getWind((char *)&text);
  drawImage(x+WEATHER_CHART_ICON_LEFT_MARGIN, y-iconSpacing, wind_icon_width, wind_icon_height, wind_icon_data);
  y += WEATHER_CHART_VALUE_Y_OFFSET;
  write_string_with_outline(&stdfont, text, &x, &y, &rightAlignedStyle);
  drawWeatherChart(chartLeftMargin, topMargin + chartSpacing,
            weather.now.wind,
            segmentSize, weather.previsions,
            [](int i, T_Snap previsions[]) {return previsions[i].wind;}
            );
  // 3rd
  x = leftMargin; y = topMargin + (chartSpacing*2);
  weather.now.getCloud((char *)&text);
  drawImage(x+WEATHER_CHART_ICON_LEFT_MARGIN, y-iconSpacing, cloud_icon_width, cloud_icon_height, cloud_icon_data);
  y += WEATHER_CHART_VALUE_Y_OFFSET;
  write_string_with_outline(&stdfont, text, &x, &y, &rightAlignedStyle);
  drawWeatherChart(chartLeftMargin, topMargin + (chartSpacing*2),
            weather.now.cloudcover/2,
            segmentSize, weather.previsions,
            [](int i, T_Snap previsions[]) {return previsions[i].cloudcover/2;}
            );
  // 4th
  x = leftMargin; y = topMargin + (chartSpacing*3);
  weather.now.getPrecipitation((char *)&text);
  drawImage(x+WEATHER_CHART_ICON_LEFT_MARGIN, y-iconSpacing, rain_icon_width, rain_icon_height, rain_icon_data);
  y += WEATHER_CHART_VALUE_Y_OFFSET;
  write_string_with_outline(&stdfont, text, &x, &y, &rightAlignedStyle);
  drawWeatherChart(chartLeftMargin, topMargin + (chartSpacing*3),
            weather.now.precipitation*10,
            segmentSize, weather.previsions,
            [](int i, T_Snap previsions[]) {return int(previsions[i].precipitation*10.0);}
            );
  cleanupWeatherData(weather);
}
#endif

void setup()
{
  basic_init();
  display_init();
  if (!config.valid || WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    epd_fullclear(&hl, temperature);
    start_settings_mode();
    settings_mode = true;
  } else {
    delay(50);
    time_init();
    epd_fullclear(&hl, temperature);
#ifdef ENABLE_BACKGROUND
    int i = random(0, NB_BACKGROUNDS);
    epd_copy_to_framebuffer(fullscreenArea, backgrounds[i], fb);
#else
    epd_hl_set_all_white(&hl);
#endif
#ifdef ENABLE_WEATHER
    show_weather();
#endif
#ifdef ENABLE_BATTERY
    correct_adc_reference();
    show_battery_level();
#endif
#ifdef ENABLE_GARBAGE
    show_garbage();
#endif
  }
  epd_poweron();
  epd_hl_update_screen(&hl, MODE_GC16, temperature);
  if (settings_mode) epd_poweroff();
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
