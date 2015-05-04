#include <pebble.h>
#include <stdlib.h>
  
/*
  Timefeld - The Seinfeld Pebble watchface.
*/
static Window *s_main_window;
static TextLayer *s_time_layer; // Time TextLayer
static TextLayer *s_quote_layer; //The quote display
static TextLayer *s_its_layer; // This TextLayer simply displays the word "It's" above the time
static GFont s_time_font; //The font for the time
static GFont s_quote_font; //The font for the time
static BitmapLayer *s_background_layer;
static GBitmap *s_background_bitmap; //The image

//An array of Seinfeld quotes.
char quote0[] = "These pretzels are making me thirsty!";
char quote1[] = "The jerk store called. They're running out of you!";
char quote2[] = "Jerry, just remember, it's not a lie if you believe it.";
char quote3[] = "Hello, Newman.";
char quote4[] = "No soup for you!";
char quote5[] = "It's a Festivus miracle!";
char quote6[] = "I think I can sum up the show for you with one word: nothing.";
char quote7[] = "They're real, and they're spectacular.";
char quote8[] = "Serenity now!";
char quote9[] = "But are you still master of your domain?";
//char quote10[] = "A George divided against itself cannot stand."
char *quotes[10] = {quote0, quote1, quote2, quote3, quote4, quote5, quote6, quote7, quote8, quote9};

//int r;

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  // Create a long-lived buffer
  static char buffer[] = "00:00";

  // Write the current hours and minutes into the buffer
  if(clock_is_24h_style()) {
    //Use 24h hour format
    strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
  } else {
    //Use 12 hour format
    strftime(buffer, sizeof("00:00"), "%I:%M", tick_time);
  }

  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, buffer);
}

static void main_window_load(Window *window) {
  //Create GBitmap, then set to created BitmapLayer
  s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_TIMEFELD);
  s_background_layer = bitmap_layer_create(GRect(0, 0, 144, 168));
  bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
	bitmap_layer_set_background_color(s_background_layer, GColorClear);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_background_layer));
  
  // Create "It's" TextLayer
  s_its_layer = text_layer_create(GRect(0, 12, 144, 50));
  text_layer_set_background_color(s_its_layer, GColorClear);
	
	#ifdef PBL_COLOR
  	text_layer_set_text_color(s_its_layer, GColorDarkCandyAppleRed);
	#else
		text_layer_set_text_color(s_its_layer, GColorBlack);
	#endif
	
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_FENICE_36));
  text_layer_set_font(s_its_layer, s_time_font);
  text_layer_set_text_alignment(s_its_layer, GTextAlignmentCenter);
  text_layer_set_text(s_its_layer, "It's");
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_its_layer));
  
  // Create time TextLayer.
  s_time_layer = text_layer_create(GRect(0, 58, 144, 50));
  text_layer_set_background_color(s_time_layer, GColorClear);
	#ifdef PBL_COLOR
  	text_layer_set_text_color(s_time_layer, GColorDarkCandyAppleRed);
	#else
		text_layer_set_text_color(s_time_layer, GColorBlack);
	#endif
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_FENICE_36));
  text_layer_set_font(s_time_layer, s_time_font);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  text_layer_set_text(s_time_layer, "00:00");
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
  
  //Create the quote TextLayer.
  s_quote_layer = text_layer_create(GRect(0, 120, 144, 50));
  s_quote_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_FENICE_28));
  text_layer_set_background_color(s_quote_layer, GColorClear);
	
	#ifdef PBL_COLOR
  	text_layer_set_text_color(s_quote_layer, GColorDarkCandyAppleRed);
	#else
		text_layer_set_text_color(s_quote_layer, GColorWhite);
	#endif
  text_layer_set_text_alignment(s_quote_layer, GTextAlignmentCenter);
  
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_quote_layer));
  
  update_time();
}

static void main_window_unload(Window *window) {
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

//The accelerometer handler. A new random quote will be displayed whenever you shake or tap the watch.
void tap_handler(AccelAxisType axis, int32_t direction) {
  int r = rand() % 10; //The int to generate the random quote
  text_layer_set_text(s_quote_layer, quotes[r]);
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
  accel_tap_service_subscribe(tap_handler);
}

static void deinit() {
  // Destroy Window
  window_destroy(s_main_window);
  accel_tap_service_unsubscribe();
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}