// Refresh every "INTERVAL" hours
#define INTERVAL 2

// feature switches

#define ENABLE_BACKGROUND

#define ENABLE_WEATHER

#define ENABLE_MOON_PHASE_IMAGE
#define ENABLE_MOON_PHASE_TEXT

#define ENABLE_BATTERY
#ifdef ENABLE_BATTERY
#define ENABLE_BATTERY_TEXT
#define ENABLE_BATTERY_IMAGE
#endif

//#define ENABLE_GARBAGE

#ifdef ENABLE_BACKGROUND

#include "images/wallpaper1.h"
#include "images/wallpaper2.h"
#include "images/wallpaper3.h"
#include "images/wallpaper4.h"

const uint8_t *backgrounds[] = {
  wallpaper1_data,
  wallpaper2_data,
  wallpaper3_data,
  wallpaper4_data,
};
#endif

// Layout / positions

#define GARBAGE_X_POSITION 890
#define GARBAGE_Y_POSITION 460

// base features layout
#define BATTERY_X_POSITION 860
#define BATTERY_Y_POSITION 5

#define MOON_PHASES_LEFT_MARGIN 100
#define MOON_PHASES_BOTTOM_MARGIN 25

#define WEATHER_TOP_MARGIN 140
#define WEATHER_LEFT_MARGIN 65
#define WEATHER_CHART_SEGMENT_SIZE 10
#define WEATHER_CHART_Y_SPACING 100
#define WEATHER_CHART_LEFT_MARGIN 150
#define WEATHER_CHART_ICON_LEFT_MARGIN 20
// relative Y offset for the text
#define WEATHER_CHART_VALUE_Y_OFFSET -10

#define WEATHER_MOON_X_POS 750
#define WEATHER_MOON_Y_POS 100
