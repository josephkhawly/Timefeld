#include <pebble.h>
#include <stdlib.h>
#include <time.h>
  
/*
  Timefeld - The watchface about nothing.
  
  basics: the clock should have the Seinfeld font. Also, think about adding randomly generated quotes.
  No custom fonts for the quotes for now.
  
  Not sure if I should add battery and date. I might need the screen real estate for the quotes
*/
static Window *s_main_window;
static TextLayer *s_time_layer; // Time
static TextLayer *s_quote_layer; //The quote display
static GFont s_time_font; //The font for the time
static GFont s_quote_font; //The font for the time
static BitmapLayer *s_background_layer;
static GBitmap *s_background_bitmap; //The image

//An array of Seinfeld quotes. A new one will be displayed every hour.
char quote0[] = "These pretzels are making me thirsty!";
char quote1[] = "The jerk store called. They're running out of you!";
char quote2[] = "Jerry, just remember, it's not a lie if you believe it.";
char quote3[] = "Hello, Newman.";
char quote4[] = "No soup for you!";
char quote5[] = "A festivus for the rest of us!";
char quote6[] = "I think I can sum up the show for you with one word: nothing.";
char quote7[] = "They're real, and they're spectacular.";
char quote8[] = "Serenity now!";
char quote9[] = "But are you still master of your domain?";
char *quotes[10] = {quote0, quote1, quote2, quote3, quote4, quote5, quote6, quote7, quote8, quote9};

int r; //The int to generate the random quote

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  // Create a long-lived buffer
  static char buffer[] = "00:00";

  // Write the current hours and minutes into the buffer
  if(clock_is_24h_style() == true) {
    //Use 2h hour format
    strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
  } else {
    //Use 12 hour format
    strftime(buffer, sizeof("00:00"), "%I:%M", tick_time);
  }
  
  //Display a new quote every hour.
  if (((tick_time->tm_min == 59) && (tick_time->tm_sec >= 52)) || ((tick_time->tm_min == 0) && (tick_time->tm_sec <= 1)) ) {
      r = rand() % 10;
    }

  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, buffer);
}

static void main_window_load(Window *window) {
  //Create GBitmap, then set to created BitmapLayer
  s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_TIMEFELD_WHITE);
  s_background_layer = bitmap_layer_create(GRect(0, 0, 144, 168));
  bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
  bitmap_layer_set_background_color(s_background_layer, GColorBlack);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_background_layer));
  
  // Create time TextLayer
  s_time_layer = text_layer_create(GRect(0, 58, 144, 50));
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorBlack);
  //Create GFont
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_FENICE_36));
  //Apply to TextLayer
  text_layer_set_font(s_time_layer, s_time_font);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  text_layer_set_text(s_time_layer, "00:00");
  // Add it as a child layer to the Window's root layer
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
  
  s_quote_layer = text_layer_create(GRect(0, 120, 144, 50));
  s_quote_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_FENICE_23));
  text_layer_set_background_color(s_quote_layer, GColorClear);
  text_layer_set_text_color(s_quote_layer, GColorWhite);
  text_layer_set_text_alignment(s_quote_layer, GTextAlignmentCenter);
  text_layer_set_text(s_quote_layer, quotes[r]);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_quote_layer));
  
  update_time();
}

static void main_window_unload(Window *window) {
  battery_state_service_unsubscribe();
  
  //Unload GFont
  fonts_unload_custom_font(s_time_font);
  fonts_unload_custom_font(s_quote_font);
  
  //Destroy GBitmap
  gbitmap_destroy(s_background_bitmap);

  //Destroy BitmapLayer
  bitmap_layer_destroy(s_background_layer);
  
  // Destroy TextLayers
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_quote_layer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}
  
static void init() {
  // Create main Window element and assign to pointer
  s_main_window = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  
  window_stack_push(s_main_window, true); // Show the Window on the watch, with animated=true
  
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler); // Register with TickTimerService
}

static void deinit() {
  // Destroy Window
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}