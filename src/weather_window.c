#include <pebble.h>
#include "main.h"
#include "weather_window.h"
#include "steps_window.h"
#include "clock_window.h"

static Window *s_main_window;
static TextLayer *s_layer1, *s_layer2;
static GFont s_font, s_small_font, s_small_font_bold;
static ActionBarLayer *s_action_bar;
static BitmapLayer *s_bitmap_layer;
static GBitmap *s_up_bitmap, *s_down_bitmap, *s_bitmap;
static TextLayer *s_city_layer, *s_temp_layer, *s_summary_layer, *s_behind_action_bar;
static AppTimer *s_timer;

static void up_click() {
  show_steps_window();
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Weather up_click");
}

static void down_click() {
  show_clock_window();
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Weather down_click");
}

static void go_home() {
  show_clock_window();
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Weather go_home");
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  app_timer_register(300, up_click, NULL); 
//   if(s_timer) { app_timer_cancel(s_timer); }
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Weather up_click_handler");
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  app_timer_register(300, down_click, NULL);
//   if(s_timer) { app_timer_cancel(s_timer); }
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Weather down_click_handler");
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Weather click_config_provider");
}

static void load_icons(Window *window) {
  
  // up is health icon
  s_up_bitmap = gbitmap_create_with_resource(RESOURCE_ID_SHOE_WHITE_ICON);
        
  // down is clock icon
  s_down_bitmap = gbitmap_create_with_resource(RESOURCE_ID_CLOCK_WHITE_ICON);
  
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
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Weather load_icons");
}  

static void window_load(Window *window) {
  hide_steps_window();
  hide_clock_window();
  
  // get information about the window
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  window_set_background_color(window, GColorWhite);   
  
  // add bar behind action bar to keep section black until action bar loads
  s_behind_action_bar = text_layer_create(GRect(114, 0, 30, 168));
  text_layer_set_background_color(s_behind_action_bar, GColorBlack);
  layer_add_child(window_layer, text_layer_get_layer(s_behind_action_bar));
  
  /////////////////
  // create font //
  /////////////////
  s_font = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
  s_small_font = fonts_get_system_font(FONT_KEY_GOTHIC_14);
  s_small_font_bold = fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD);
    
  ////////////////////////////////////
  // create text layer for the city //
  ////////////////////////////////////
  s_city_layer = text_layer_create(GRect(0, 2, bounds.size.w-ACTION_BAR_WIDTH, 20));
  text_layer_set_text_alignment(s_city_layer, GTextAlignmentCenter);
  text_layer_set_font(s_city_layer, s_font);
  layer_add_child(window_layer, text_layer_get_layer(s_city_layer));    
  
  // set city, already obtained from clock_window.c weather call to app.js
  text_layer_set_text(s_city_layer, city_layer_buf); 
  
  ///////////////////////////////////////////
  // create text layer for the temperature //
  ///////////////////////////////////////////
  s_temp_layer = text_layer_create(GRect(0, 22, bounds.size.w-ACTION_BAR_WIDTH, 16));
  text_layer_set_text_alignment(s_temp_layer, GTextAlignmentCenter);
  text_layer_set_font(s_temp_layer, s_small_font);
  layer_add_child(window_layer, text_layer_get_layer(s_temp_layer));  
  
  // set tempex, already obtained from clock_window.c weather call to app.js
  text_layer_set_text(s_temp_layer, tempex_layer_buf);
  
  ////////////////
  // icon layer //
  ////////////////
  switch(icon) {
    case 0:
      // clear-day
      s_bitmap = gbitmap_create_with_resource(RESOURCE_ID_CLEAR_SKY_DAY_BLACK_ICON);  
      break;
    case 1:
      // clear-night
      s_bitmap = gbitmap_create_with_resource(RESOURCE_ID_CLEAR_SKY_NIGHT_BLACK_ICON);  
      break;
    case 2:
      // rain
      s_bitmap = gbitmap_create_with_resource(RESOURCE_ID_RAIN_BLACK_ICON);  
      break;
    case 3:
      // snow
      s_bitmap = gbitmap_create_with_resource(RESOURCE_ID_SNOW_BLACK_ICON);  
      break;
    case 4:
      // sleet
      s_bitmap = gbitmap_create_with_resource(RESOURCE_ID_SLEET_BLACK_ICON);  
      break;
    case 5:
      //wind
      s_bitmap = gbitmap_create_with_resource(RESOURCE_ID_WIND_BLACK_ICON);  
      break;
    case 6:
      // fog
      s_bitmap = gbitmap_create_with_resource(RESOURCE_ID_FOG_BLACK_ICON);  
      break;
    case 7:
      // cloudy
      s_bitmap = gbitmap_create_with_resource(RESOURCE_ID_CLOUDY_BLACK_ICON);  
      break;
    case 8:
      // partly-cloudy-day
      s_bitmap = gbitmap_create_with_resource(RESOURCE_ID_PARTLY_CLOUDY_DAY_BLACK_ICON);  
      break;
    case 9:
      // partly-cloudy-night
      s_bitmap = gbitmap_create_with_resource(RESOURCE_ID_PARTLY_CLOUDY_NIGHT_BLACK_ICON);  
      break;
  }
  
  // draw weather icon
  s_bitmap_layer = bitmap_layer_create(GRect(0, 38, bounds.size.w-ACTION_BAR_WIDTH, 40));
  bitmap_layer_set_compositing_mode(s_bitmap_layer, GCompOpSet);
  bitmap_layer_set_bitmap(s_bitmap_layer, s_bitmap); 
  layer_add_child(window_layer, bitmap_layer_get_layer(s_bitmap_layer));  
  
  // create text layer for the summary
  s_summary_layer = text_layer_create(GRect(0, 78, bounds.size.w-ACTION_BAR_WIDTH, 90));
  text_layer_set_text_alignment(s_summary_layer, GTextAlignmentCenter);
  text_layer_set_font(s_summary_layer, s_small_font);
  layer_add_child(window_layer, text_layer_get_layer(s_summary_layer));    
  
  // set summary, already obtained from clock_window.c weather call to app.js
  text_layer_set_text(s_summary_layer, summary_layer_buf);
  
  // load icons
  load_icons(window);
   
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Weather window_load");
}

static void window_unload(Window *window) {
  app_timer_cancel(s_timer);
  text_layer_destroy(s_layer1);
  text_layer_destroy(s_layer2);
  action_bar_layer_destroy(s_action_bar);
  gbitmap_destroy(s_up_bitmap);
  gbitmap_destroy(s_down_bitmap);  
  gbitmap_destroy(s_bitmap); 
  text_layer_destroy(s_city_layer);
  text_layer_destroy(s_temp_layer);
  text_layer_destroy(s_summary_layer);
  text_layer_destroy(s_behind_action_bar);
  window_destroy(s_main_window);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Weather window_unload");  
}

void show_weather_window(void) {
  s_main_window = window_create();
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload
  });
  window_stack_push(s_main_window, true);
  
  // start timer
  s_timer = app_timer_register(20000, go_home, NULL);  
      
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Weather show_weather_window");
}

void hide_weather_window(void) {
  window_stack_remove(s_main_window, true);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Weather hide_weather_window");
}