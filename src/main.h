#pragma once
#define KEY_CITY 0
#define KEY_TEMP 1
#define KEY_SUMMARY 2
#define KEY_CURRENT_SUMMARY 3
#define KEY_TEMP_EXTENDED 4
#define KEY_ICON 5

int icon;

char city_layer_buf[32];
char temp_layer_buf[32];
char summary_layer_buf[128];
char tempex_layer_buf[32];
char icon_layer_buf[32];  