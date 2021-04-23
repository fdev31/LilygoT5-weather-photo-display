#include <Arduino.h>
#include <WebServer.h>
#include <EEPROM.h>
#include "configserver.h"
#include <display.h>

WebServer server(80);

void index_html() {
    if (server.method() != HTTP_POST)
        server.send(200, "text/html", "<!DOCTYPE html><meta name=\"viewport\" content=\"width=device-width, initial-scale=2.0\">"
"<html><body>"
"<h1>WiFi settings:</h1>"
"<form method=\"post\" action=\"/\">"
"<input type=\"text\" name=\"ssid\">SSID</input>"
"<br/><input type=\"password\" name=\"password\">pass</input>"
"<br/><input type=\"submit\" value=\"Submit\">"
"</form>"
"</body></html>");
    else {
        String new_ssid = "";
        String new_password = "";

        for (uint8_t i = 0; i < server.args(); i++)
        {
            if (server.argName(i) == "ssid") {
                new_ssid = server.arg(i);
            } else if(server.argName(i) == "password") {
                new_password = server.arg(i);
            }
        }

        server.send(200, "text/plain", "saved");
        int d=0;
        int i=0;
        EEPROM.write(d++, 42); /* magic number */
        do {
            EEPROM.write(d++, new_ssid[i]);
        } while (new_ssid[i++]);
        i=0;
        do {
            EEPROM.write(d++, new_password[i]);
        } while (new_password[i++]);
        EEPROM.commit();
        delay(200);
        esp_restart();
    }
}

void start_settings_mode() {
    const char* ssid = "Weather-paper";  // Enter SSID here
    const char* password = "12345678";  //Enter Password here

    IPAddress local_ip(192,168,1,1);
    IPAddress gateway(192,168,1,1);
    IPAddress subnet(255,255,255,0);

    WiFi.persistent(false);
    WiFi.softAP(ssid, password);

    display.setCursor(10, 50);
    display.setFontType(1);
    display.println("Connection failed, please connect your wifi:\n");
    display.print("Access point: ");
    display.setFontType(2);
    display.println(ssid);
    display.setFontType(1);
    display.print("Password: ");
    display.setFontType(2);
    display.println(password);
    display.setFontType(1);
    display.println("... and then open:");
    display.setFontType(0);
    display.println("http://192.168.1.1/");
    display.setFontType(1);
    display.setCursor(10, EPD_HEIGHT - 10);
    display.println("... to enter your local WiFi credentials");
    display.update();
    delay(2000);
    WiFi.softAPConfig(local_ip, gateway, subnet);
    server.on("/", index_html);
    server.begin();
}
void handle_configserver() {
    server.handleClient();
}
Configuration init_config() {
    Configuration cfg;
    EEPROM.begin(200);
    if (EEPROM.read(0) == 42)
    {
        cfg.valid = true;
        int s = 1;
        int i = 0;
        do {
            cfg.ssid[i++] = EEPROM.read(s++);
        } while (cfg.ssid[i - 1] && s < 200);
        i = 0;
        do {
            cfg.pass[i++] = EEPROM.read(s++);
        } while (cfg.pass[i - 1] && s < 200);
  } else {
      cfg.valid = false;
  }
  return cfg;
}