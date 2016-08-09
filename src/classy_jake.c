#include <pebble.h>
#include "main.h"
#include "clock_window.h"
#include "weather_window.h"
#include "steps_window.h"

int icon;
char city_layer_buf[32];
char temp_layer_buf[32];
char summary_layer_buf[128]; 
char tempex_layer_buf[32];
char icon_layer_buf[32];
GBitmap *s_up_bitmap, *s_down_bitmap;
ActionBarLayer *s_action_bar;
AppTimer *s_timer;

// for weather calls
static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  // Store incoming information
  static char city_buf[32];
  static char temp_buf[8];
  static char summary_buf[128];
  static char tempex_buf[32];  
  static char icon_buf[32];

  // Read tuples for data
  Tuple *city_tuple = dict_find(iterator, KEY_CITY);
  Tuple *temp_tuple = dict_find(iterator, KEY_TEMP);
  Tuple *summary_tuple = dict_find(iterator, KEY_SUMMARY);
  Tuple *tempex_tuple = dict_find(iterator, KEY_TEMP_EXTENDED);    
  Tuple *icon_tuple = dict_find(iterator, KEY_ICON);  

  // If all data is available, use it
  if(city_tuple && temp_tuple && summary_tuple && tempex_tuple && icon_tuple ) {
    
    // city
    snprintf(city_buf, sizeof(city_buf), "%s", city_tuple->value->cstring);
    snprintf(city_layer_buf, sizeof(city_layer_buf), "%s", city_buf);
    
    // temp
    snprintf(temp_buf, sizeof(temp_buf), "%d°", (int)temp_tuple->value->int32);
    snprintf(temp_layer_buf, sizeof(temp_layer_buf), "%s", temp_buf);
    
    // summary
    snprintf(summary_buf, sizeof(summary_buf), "%s", summary_tuple->value->cstring);
    snprintf(summary_layer_buf, sizeof(summary_layer_buf), "%s", summary_buf);    
    
    // tempex
    snprintf(tempex_buf, sizeof(tempex_buf), "%s", tempex_tuple->value->cstring);
    snprintf(tempex_layer_buf, sizeof(tempex_layer_buf), "%s", tempex_buf);    

    // icon
    snprintf(icon_buf, sizeof(icon_buf), "%s", icon_tuple->value->cstring);
    snprintf(icon_layer_buf, sizeof(icon_layer_buf), "%s", icon_buf);    
    
    // populate icon variable
    if(strcmp(icon_layer_buf, "clear-day")==0) {
      icon = 0;
    } else if(strcmp(icon_layer_buf, "clear-night")==0) {
      icon = 1;
    }else if(strcmp(icon_layer_buf, "rain")==0) {
      icon = 2;
    } else if(strcmp(icon_layer_buf, "snow")==0) {
      icon = 3;
    } else if(strcmp(icon_layer_buf, "sleet")==0) {
      icon = 4;
    } else if(strcmp(icon_layer_buf, "wind")==0) {
      icon = 5;
    } else if(strcmp(icon_layer_buf, "fog")==0) {
      icon = 6;
    } else if(strcmp(icon_layer_buf, "cloudy")==0) {
      icon = 7;
    } else if(strcmp(icon_layer_buf, "partly-cloudy-day")==0) {
      icon = 8;
    } else if(strcmp(icon_layer_buf, "partly-cloudy-night")==0) {
      icon = 9;
    }
  }
  
  // load default icon
//   load_icons(s_main_window);
  
  APP_LOG(APP_LOG_LEVEL_INFO, "Clock inbox_received_callback");
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

static void up_click() {
  show_weather_window();
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Clock up_click");
}

static void down_click() {
  show_steps_window();
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Clock down_click");
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  app_timer_register(300, up_click, NULL);
  if(s_timer) { app_timer_cancel(s_timer); }
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Clock up_click_handler");
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  app_timer_register(300, down_click, NULL);
  if(s_timer) { app_timer_cancel(s_timer); }
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Clock down_click_handler");
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Clock click_config_provider");
}

// loads icons for action bar
static void get_icons(Window *window, int screen) {
  
  switch(screen) {
    // clock
    case 0:
      // up is weather
      switch(icon) {
        case 0:
          // clear-day
          s_up_bitmap = gbitmap_create_with_resource(RESOURCE_ID_CLEAR_SKY_DAY_WHITE_ICON);  
          break;
        case 1:
          // clear-night
          s_up_bitmap = gbitmap_create_with_resource(RESOURCE_ID_CLEAR_SKY_NIGHT_WHITE_ICON);  
          break;
        case 2:
          // rain
          s_up_bitmap = gbitmap_create_with_resource(RESOURCE_ID_RAIN_WHITE_ICON);  
          break;
        case 3:
          // snow
          s_up_bitmap = gbitmap_create_with_resource(RESOURCE_ID_SNOW_WHITE_ICON);  
          break;
        case 4:
          // sleet
          s_up_bitmap = gbitmap_create_with_resource(RESOURCE_ID_SLEET_WHITE_ICON);  
          break;
        case 5:
          //wind
          s_up_bitmap = gbitmap_create_with_resource(RESOURCE_ID_WIND_WHITE_ICON);  
          break;
        case 6:
          // fog
          s_up_bitmap = gbitmap_create_with_resource(RESOURCE_ID_FOG_WHITE_ICON);  
          break;
        case 7:
          // cloudy
          s_up_bitmap = gbitmap_create_with_resource(RESOURCE_ID_CLOUDY_WHITE_ICON);  
          break;
        case 8:
          // partly-cloudy-day
          s_up_bitmap = gbitmap_create_with_resource(RESOURCE_ID_PARTLY_CLOUDY_DAY_WHITE_ICON);  
          break;
        case 9:
          // partly-cloudy-night
          s_up_bitmap = gbitmap_create_with_resource(RESOURCE_ID_PARTLY_CLOUDY_NIGHT_WHITE_ICON);  
          break;
        default:
          s_up_bitmap = gbitmap_create_with_resource(RESOURCE_ID_OTHER_WHITE_ICON);  
          break;
      }  
      text_layer_set_text(s_temp_layer, temp_layer_buf);
      text_layer_set_text(s_city_layer, city_layer_buf);  
      break;
    case 1:
      break;
    case 2:
      break;
  }
  
  // down is health
  s_down_bitmap = gbitmap_create_with_resource(RESOURCE_ID_SHOE_WHITE_ICON);
  
  // Create ActionBarLayer
  s_action_bar = action_bar_layer_create();  
  
  // Set the icons
  action_bar_layer_set_icon(s_action_bar, BUTTON_ID_UP, s_up_bitmap);
  action_bar_layer_set_icon(s_action_bar, BUTTON_ID_DOWN, s_down_bitmap);
  
  // animate button press
  action_bar_layer_set_icon_press_animation(s_action_bar, BUTTON_ID_UP, ActionBarLayerIconPressAnimationMoveDown);
  action_bar_layer_set_icon_press_animation(s_action_bar, BUTTON_ID_DOWN, ActionBarLayerIconPressAnimationMoveUp);
  
  // Add to Window
  action_bar_layer_add_to_window(s_action_bar, window);
  action_bar_layer_set_click_config_provider(s_action_bar, click_config_provider);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Clock load_icons");
}  
