#include <Arduino.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include "display.h"
#include "garbage.h"

void dumpDate(const char *prefix, struct tm d);

char *pickupText = NULL, _pickupText[100];

char today[11], end_date[11], tomorrow[11];
extern struct tm timeinfo; // today's date

void makePickupText(const char *type, const char *date,  struct tm today) {
    struct tm pickup_date;
    time_t pickup_ts, today_ts;

    today_ts = mktime(&today);

    memcpy(&pickup_date, &today, sizeof(struct tm));

    int day, month, year;
    sscanf(date, "%4d-%02d-%02d", &year, &month, &day);
    pickup_date.tm_year = year - 1900;
    pickup_date.tm_mon = month - 1;
    pickup_date.tm_mday = day;
    pickup_ts = mktime(&pickup_date);

    int delta = (pickup_ts - today_ts)/86400; // count days

    switch(delta) {
        case 0:
            sprintf(_pickupText, "in");
            break;
        case 1:
            sprintf(_pickupText, "out");
            break;
        default:
            sprintf(_pickupText, "%d", delta);
    }
}
void free_garbage (GarbageDays *gd) {
  if(gd->std) free(gd->std);
  if(gd->paper) free(gd->paper);
}

GarbageDays check_garbage(struct tm timeinfo) {
  char *img = NULL;
  struct tm tmptimeinfo;
  GarbageDays ret;
  sprintf(today, "%d-%02d-%02d", timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday);
  memcpy(&tmptimeinfo, &timeinfo, sizeof(struct tm));

  dumpDate("Today", tmptimeinfo);

  /* tomorrow */
  tmptimeinfo.tm_mday += 1;
  mktime(&tmptimeinfo);
  sprintf(tomorrow, "%d-%02d-%02d", tmptimeinfo.tm_year + 1900, tmptimeinfo.tm_mon + 1, tmptimeinfo.tm_mday);

  /* end date */
  tmptimeinfo.tm_mday += 15; /* 15 days lookup */
  mktime(&tmptimeinfo);
  sprintf(end_date, "%d-%02d-%02d", tmptimeinfo.tm_year + 1900, tmptimeinfo.tm_mon + 1, tmptimeinfo.tm_mday);

  /* garbage pickup */
  DynamicJsonDocument doc(1024);
  //    char **img = NULL;
  HTTPClient http;

  http.begin("https://wasteprod2api.ximmio.com/api/GetCalendar");
  http.addHeader("Content-Type", "application/json");
  String httpRequestData = (String) "{community:'Aalsmeer',companyCode:'800bf8d7-6dd1-4490-ba9d-b419d6dc8a45',uniqueAddressID:'1000083693',startDate:'" + (char *)today + "',endDate:'" + (char *)end_date + "'}";
  int httpCode = http.POST(httpRequestData);
  if (httpCode == 200)
  {
    deserializeJson(doc, http.getString());
    int pickupType;
    char *pickup_date = NULL;

    for (const JsonObject &entry : doc["dataList"].as<JsonArray>())
    {
      pickupType = entry["pickupType"];
      const char *tdate = entry["pickupDates"][0];
      switch (pickupType)
      {
      case 2:
        if (pickup_date == NULL || strcmp(pickup_date, tdate) > 0)
        {
          pickup_date = (char *)tdate;
          makePickupText("Paper", pickup_date, timeinfo);
          if (ret.paper) free(ret.paper);
          ret.paper = strdup(_pickupText);
        }
        break;
      case 0:
        if (pickup_date == NULL || strcmp(pickup_date, tdate) > 0)
        {
          pickup_date = (char *)tdate;
          makePickupText("General", pickup_date, timeinfo);
          if(ret.std) free(ret.std);
          ret.std = strdup(_pickupText);
        }
        break;
      }
    }
  }
  http.end();
  if (img) ret.image = img;
  return ret;
}