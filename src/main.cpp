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
  double level = get_battery_level();
  int w = (level - 3.0)*47.5; // 57 pixels width, 1.2 == 100%
  if (w > 57) w = 57;
  else if (w < 0) w = 0;
  char text[50];
  display.setFontType(1);
  sprintf(text, "%.2fV", level);
  display.setCursor(BATTERY_POS, 36);
  display.print(text);
  if (w) {
    EpdRect battery_rect = {.x = 870, .y = 17, .width = w, .height = 15};
    epd_fill_rect(battery_rect, 1, fb);
  }
}

void show_garbage() {
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

void drawChart(int x, int y, int initVal, int increment, T_Snap previsions[], int (*getFunc)(int, T_Snap[])) {
  int i=0;
  int prevVal = y-initVal;

  epd_draw_line(x, prevVal, x + 3*7*increment, prevVal, 127, fb);
  for (int daycount=0; daycount<3; daycount++) {
    for (int snapcount=0; snapcount<7; snapcount++) {
      int val = y-getFunc(i, previsions);
      epd_draw_line(x, prevVal, x+increment, val, 0, fb);
      prevVal = val;
      x+=increment;
      i++;
    }
  }
}

void show_weather() {
  WeatherSnapshot weather = getWeather();
  int x = BATTERY_POS/2, y = 70;
  char text[50];

  epd_write_string(&titlefont, weather.title, &x, &y, fb, &centeredStyle);

  // right aligned captions
  int sz=10;


  x = 150; y = 120;
  for (int daycount=0; daycount<3; daycount++) {
    epd_draw_line(x, y-20, x, y+130, 127, fb);
    x+= 7*sz;
  }
  x = 65;
  weather.now.getTemperature((char *)&text);
  epd_write_string(&stdfont, text, &x, &y, fb, &rightAlignedStyle);
  drawChart(150, 120,
            weather.now.temperature*2,
            sz, weather.previsions,
            [](int i, T_Snap previsions[]) {return previsions[i].temperature*2;}
            );
  x = 65; y = 180;
  weather.now.getWind((char *)&text);
  epd_write_string(&stdfont, text, &x, &y, fb, &rightAlignedStyle);
  drawChart(150, 230,
            weather.now.wind,
            sz, weather.previsions,
            [](int i, T_Snap previsions[]) {return previsions[i].wind;}
            );
  x = 65; y = 230;
  weather.now.getCloud((char *)&text);
  epd_write_string(&stdfont, text, &x, &y, fb, &rightAlignedStyle);
  drawChart(150, 185,
            weather.now.cloudcover/2,
            sz, weather.previsions,
            [](int i, T_Snap previsions[]) {return previsions[i].cloudcover/2;}
            );
#if 0
      y = 120;
  int i=0;
  x += 100;
  int prevVal = y - (2*weather.now.temperature);
  for (int daycount=0; daycount<3; daycount++) {
    epd_draw_line(x, y-20, x, y+130, 15, fb);
    for (int snapcount=0; snapcount<7; snapcount++) {
      int val = y-(weather.previsions[i].temperature*2);
      epd_draw_line(x, prevVal, x+sz, val, 0, fb);
      prevVal = val;
      x+=sz;
      i++;
    }
  }

  x = 65; y = 180;
  weather.now.getWind((char *)&text);
  epd_write_string(&stdfont, text, &x, &y, fb, &rightAlignedStyle);
  y = 180;
  i = 0;
  x += 100;
  prevVal = y - weather.now.wind;
  for (int daycount=0; daycount<3; daycount++)
    for (int snapcount=0; snapcount<7; snapcount++) {
      int val = y-weather.previsions[i].wind;
      epd_draw_line(x, prevVal, x+sz, val, 0, fb);
      prevVal = val;
      x+=sz;
      i++;
    }

  x = 65; y = 230;
  weather.now.getCloud((char *)&text);
  epd_write_string(&stdfont, text, &x, &y, fb, &rightAlignedStyle);
  y = 230;
  i = 0;
  x += 100;
  prevVal = y - (weather.now.cloudcover / 2);
  for (int daycount=0; daycount<3; daycount++)
    for (int snapcount=0; snapcount<7; snapcount++) {
      int val = y-(weather.previsions[i].cloudcover/2);
      epd_draw_line(x, prevVal, x+sz, val, 0, fb);
      prevVal = val;
      x+=sz;
      i++;
    }
#endif
}

void setup()
{
  basic_init();
  display_init();
  epd_poweron();
  epd_fullclear(&hl, temperature);
  if (!config.valid || WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    Serial.println("Invalid configuration or no connexion, running settings...");
    start_settings_mode();
    settings_mode = true;
  } else {
    delay(50);
    time_init();
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
