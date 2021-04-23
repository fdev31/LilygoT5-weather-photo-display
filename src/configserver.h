#ifndef _CONFIGSERVER_H
#define _CONFIGSERVER_H

typedef struct {
    char ssid[40];
    char pass[80];
    bool valid;
} Configuration;

void start_settings_mode();
void handle_configserver() ;
Configuration init_config();


#endif