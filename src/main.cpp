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

// images
//#include "background.h"
#include "images/battery.h"
#include "images/paper_icon.h"
#include "images/recycle_icon.h"

//#include "images/icon.h"
#include "images/temp_icon.h"
#include "images/wind_icon.h"
#include "images/cloud_icon.h"
#include "images/rain_icon.h"
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

#include "images/wallpaper1.h"
//#include "images/wallpaper2.h"
//#include "images/wallpaper3.h"
#include "images/wallpaper4.h"
//#include "images/wallpaper5.h"

const uint8_t *backgrounds[] = {
  wallpaper1_data,
//  wallpaper2_data,
//  wallpaper3_data,
  wallpaper4_data,
//  wallpaper5_data,
};

#define NB_BACKGROUNDS 2

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
void write_string_with_outline(const EpdFont *font, const char *text, int *x, int *y, const EpdFontProperties *props) {
  int xx = *x-2;
  int yy = *y-2;

  EpdFontProperties backProps = EpdFontProperties(*props);

  if (props->fg_color == BLACK) {
    backProps.fg_color = WHITE;
  } else {
    backProps.fg_color = BLACK;
  }

    epd_write_string(font, text, &xx, &yy, fb, &backProps);
    xx = *x+2;
    yy = *y+2;
    epd_write_string(font, text, &xx, &yy, fb, &backProps);

    xx = *x + 2;
    yy = *y - 2;
    epd_write_string(font, text, &xx, &yy, fb, &backProps);

    xx = *x - 2;
    yy = *y + 2;
    epd_write_string(font, text, &xx, &yy, fb, &backProps);

    epd_write_string(font, text, x, y, fb, props);
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

void drawImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint8_t *image) {
  EpdRect area = {.x=x, .y=y, .width=w, .height=h-1};
  /*
  Serial.print("Corner Color=");
  Serial.println(epd_get_pixel(0, 0, w, h, image));
  */
  epd_draw_rotated_transparent_image(area, image, fb, 64);
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
  int x = BATTERY_X_POSITION;
  int y = 35 + BATTERY_Y_POSITION;

  drawImage(x+5, BATTERY_Y_POSITION, battery_width, battery_height, battery_data);

  write_string_with_outline(&stdfont, text, &x, &y, &rightAlignedStyle);
  if (w) {
    EpdRect battery_rect = {.x = BATTERY_X_POSITION+10, .y = 5+BATTERY_Y_POSITION, .width = w, .height = 30 };
    epd_fill_rect(battery_rect, 1, fb);
  }
}

void show_garbage() {
  Serial.println("Show garbage pickup");
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
  Serial.println("Show weather");
  WeatherSnapshot weather = getWeather();
  if(!weather.title) return;
  Serial.println("Received weather data");
  int x = BATTERY_POS/2, y = 70;
  char text[50];

  write_string_with_outline(&titlefont, weather.title, &x, &y, &centeredStyle);

  x=6;
  y = EPD_HEIGHT-MOON_PHASES_BOTTOM_MARGIN;
  EpdFontProperties dateStyle = { WHITE, 8, '?', EPD_DRAW_ALIGN_LEFT};
  write_string_with_outline(&stdfont, weather.observationDate, &x, &y, &dateStyle);
  y = EPD_HEIGHT-MOON_PHASES_BOTTOM_MARGIN;
  x += MOON_PHASES_LEFT_MARGIN;
  write_string_with_outline(&stdfont, weather.moonphase, &x, &y, &dateStyle);

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

void setup()
{
  basic_init();
  display_init();
  if (!config.valid || WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    Serial.println("Invalid configuration or no connexion, running settings...");
    epd_poweron();
    epd_fullclear(&hl, temperature);
    start_settings_mode();
    settings_mode = true;
  } else {
    delay(50);
    time_init();
    epd_fullclear(&hl, temperature);
    int i = random(0, NB_BACKGROUNDS);
    Serial.print("Background:");
    Serial.println(i);
    epd_copy_to_framebuffer(fullscreenArea, backgrounds[i], fb);
    show_weather();
    correct_adc_reference();
    show_battery_level();
#ifdef ENABLE_GARBAGE
    show_garbage();
#endif
    epd_poweron();
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
