#include <pebble.h>
#include "classy_jake.c"
#include "main.h"
#include "weather_window.h"
#include "steps_window.h"
#include "clock_window.h"

static Window *s_main_window;
static BitmapLayer *s_bitmap_layer;
static GBitmap *s_bitmap;
static GFont s_font, s_small_font;
// static ActionBarLayer *s_action_bar;
static TextLayer *s_clock_layer, *s_date_layer, *s_temp_layer, *s_city_layer, *s_behind_action_bar;
// static AppTimer *s_timer;

// static void up_click() {
//   show_weather_window();
//   APP_LOG(APP_LOG_LEVEL_DEBUG, "Clock up_click");
// }

// static void down_click() {
//   show_steps_window();
//   APP_LOG(APP_LOG_LEVEL_DEBUG, "Clock down_click");
// }

// static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
//   app_timer_register(300, up_click, NULL);
//   if(s_timer) { app_timer_cancel(s_timer); }
//   APP_LOG(APP_LOG_LEVEL_DEBUG, "Clock up_click_handler");
// }

// static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
//   app_timer_register(300, down_click, NULL);
//   if(s_timer) { app_timer_cancel(s_timer); }
//   APP_LOG(APP_LOG_LEVEL_DEBUG, "Clock down_click_handler");
// }

// static void click_config_provider(void *context) {
//   window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
//   window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
//   APP_LOG(APP_LOG_LEVEL_DEBUG, "Clock click_config_provider");
// }

// loads icons for action bar
static void load_icons(Window *window) {
  get_icons(window, 0);
     
//   // up is weather
//   switch(icon) {
//     case 0:
//       // clear-day
//       s_up_bitmap = gbitmap_create_with_resource(RESOURCE_ID_CLEAR_SKY_DAY_WHITE_ICON);  
//       break;
//     case 1:
//       // clear-night
//       s_up_bitmap = gbitmap_create_with_resource(RESOURCE_ID_CLEAR_SKY_NIGHT_WHITE_ICON);  
//       break;
//     case 2:
//       // rain
//       s_up_bitmap = gbitmap_create_with_resource(RESOURCE_ID_RAIN_WHITE_ICON);  
//       break;
//     case 3:
//       // snow
//       s_up_bitmap = gbitmap_create_with_resource(RESOURCE_ID_SNOW_WHITE_ICON);  
//       break;
//     case 4:
//       // sleet
//       s_up_bitmap = gbitmap_create_with_resource(RESOURCE_ID_SLEET_WHITE_ICON);  
//       break;
//     case 5:
//       //wind
//       s_up_bitmap = gbitmap_create_with_resource(RESOURCE_ID_WIND_WHITE_ICON);  
//       break;
//     case 6:
//       // fog
//       s_up_bitmap = gbitmap_create_with_resource(RESOURCE_ID_FOG_WHITE_ICON);  
//       break;
//     case 7:
//       // cloudy
//       s_up_bitmap = gbitmap_create_with_resource(RESOURCE_ID_CLOUDY_WHITE_ICON);  
//       break;
//     case 8:
//       // partly-cloudy-day
//       s_up_bitmap = gbitmap_create_with_resource(RESOURCE_ID_PARTLY_CLOUDY_DAY_WHITE_ICON);  
//       break;
//     case 9:
//       // partly-cloudy-night
//       s_up_bitmap = gbitmap_create_with_resource(RESOURCE_ID_PARTLY_CLOUDY_NIGHT_WHITE_ICON);  
//       break;
//     default:
//       s_up_bitmap = gbitmap_create_with_resource(RESOURCE_ID_OTHER_WHITE_ICON);  
//       break;
//   }  
  
//   // down is health
//   s_down_bitmap = gbitmap_create_with_resource(RESOURCE_ID_SHOE_WHITE_ICON);
  
  text_layer_set_text(s_temp_layer, temp_layer_buf);
  text_layer_set_text(s_city_layer, city_layer_buf);  
  
//   // Create ActionBarLayer
//   s_action_bar = action_bar_layer_create();  
  
//   // Set the icons
//   action_bar_layer_set_icon(s_action_bar, BUTTON_ID_UP, s_up_bitmap);
//   action_bar_layer_set_icon(s_action_bar, BUTTON_ID_DOWN, s_down_bitmap);
  
//   // animate button press
//   action_bar_layer_set_icon_press_animation(s_action_bar, BUTTON_ID_UP, ActionBarLayerIconPressAnimationMoveDown);
//   action_bar_layer_set_icon_press_animation(s_action_bar, BUTTON_ID_DOWN, ActionBarLayerIconPressAnimationMoveUp);
  
//   // Add to Window
//   action_bar_layer_add_to_window(s_action_bar, window);
//   action_bar_layer_set_click_config_provider(s_action_bar, click_config_provider);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Clock load_icons");
}  

static void window_load(Window *window) {
  hide_steps_window();
  hide_weather_window();
  
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
  s_font = fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD);
  s_small_font = fonts_get_system_font(FONT_KEY_GOTHIC_14);
  
  ///////////////////////////////////////////
  // create text layer for the temperature //
  ///////////////////////////////////////////
  s_temp_layer = text_layer_create(GRect(0, 2, bounds.size.w-ACTION_BAR_WIDTH, 16));
  text_layer_set_text_alignment(s_temp_layer, GTextAlignmentCenter);
  text_layer_set_font(s_temp_layer, s_small_font);
  text_layer_set_text(s_temp_layer, "Fetching...");
  layer_add_child(window_layer, text_layer_get_layer(s_temp_layer));  
  
  ////////////////////////////////////
  // create text layer for the city //
  ////////////////////////////////////
  s_city_layer = text_layer_create(GRect(0, 18, bounds.size.w-ACTION_BAR_WIDTH, 16));
  text_layer_set_text_alignment(s_city_layer, GTextAlignmentCenter);
  text_layer_set_font(s_city_layer, s_small_font);
  layer_add_child(window_layer, text_layer_get_layer(s_city_layer));  
   
  /////////////////////////////////
  // create text layer for clock //
  /////////////////////////////////
  s_clock_layer = text_layer_create(GRect(0, 120, bounds.size.w-ACTION_BAR_WIDTH, 30));
  text_layer_set_text_alignment(s_clock_layer, GTextAlignmentCenter);
  text_layer_set_font(s_clock_layer, s_font);
  layer_add_child(window_layer, text_layer_get_layer(s_clock_layer));  
  
  ////////////////////////////////////////////////////////////////
  // create text layer for day of week, month, and day of month //
  ////////////////////////////////////////////////////////////////
  s_date_layer = text_layer_create(GRect(0, 148, bounds.size.w-ACTION_BAR_WIDTH, 18));
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);
  text_layer_set_font(s_date_layer, s_small_font);
  layer_add_child(window_layer, text_layer_get_layer(s_date_layer));  
  
  /////////////////////
  // create A&M Logo //
  /////////////////////
  s_bitmap = gbitmap_create_with_resource(RESOURCE_ID_LOGO_BLACK);
  s_bitmap_layer = bitmap_layer_create(GRect(0, 33, bounds.size.w-ACTION_BAR_WIDTH, 101));
  bitmap_layer_set_compositing_mode(s_bitmap_layer, GCompOpSet);
  bitmap_layer_set_bitmap(s_bitmap_layer, s_bitmap); 
  layer_add_child(window_layer, bitmap_layer_get_layer(s_bitmap_layer));  
   
  // force icons
  load_icons(window);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Clock window_load");
}

static void window_unload(Window *window) {
  bitmap_layer_destroy(s_bitmap_layer);
  gbitmap_destroy(s_bitmap);  
  gbitmap_destroy(s_up_bitmap);
  gbitmap_destroy(s_down_bitmap);
  action_bar_layer_destroy(s_action_bar);
  text_layer_destroy(s_clock_layer);
  text_layer_destroy(s_date_layer);
  text_layer_destroy(s_temp_layer);
  text_layer_destroy(s_city_layer);  
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Clock window_unload");
}

static void update_time() {
  // get a tm strucutre
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  
  // write the time into a buffer
  static char s_time_buffer[8];
  strftime(s_time_buffer, sizeof(s_time_buffer), clock_is_24h_style() ? "%H:%M" : "%l:%M", tick_time);
    
  // write day of week and day of month to buffer
  static char s_date_buffer[32];
  strftime(s_date_buffer, sizeof(s_date_buffer), "%A, %B%e", tick_time);
  
  // display this time on the text layer
//   text_layer_set_text(s_clock_layer, s_time_buffer+(('0' == s_time_buffer[0])?1:0));
  text_layer_set_text(s_clock_layer, s_time_buffer);
  text_layer_set_text(s_date_layer, s_date_buffer);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Clock update_time");
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
  
  // Get weather update every 30 minutes
  if(tick_time->tm_min % 30 == 0) {  
    // Begin dictionary
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);
  
    // Add a key-value pair
    dict_write_uint8(iter, 0, 0);
  
    // Send the message!
    app_message_outbox_send();
  }
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Clock tick_handler");
}

void show_clock_window(void) {
  s_main_window = window_create();
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload
  });
  window_stack_push(s_main_window, true);
  
  // Make sure the time is displayed from the start
  update_time();

  // register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);  
  
  // Register weather callbacks
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);  
  
  // Open AppMessage for weather callbacks
  const int inbox_size = 256;
  const int outbox_size = 256;
  app_message_open(inbox_size, outbox_size);  
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Clock show_clock_window");
}

void hide_clock_window(void) {
  window_stack_remove(s_main_window, true);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Clock hide_clock_window");
}