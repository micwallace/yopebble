#include <pebble.h>

static Window *window;
static TextLayer *text_layer;
static DictationSession *session;

static void out_sent_handler(DictionaryIterator *sent, void *context) {
  
}

static void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context) {
  
}

static void dictation_session_callback(DictationSession *session, DictationSessionStatus status, char *transcription, void *context){
  // send the message to the phone if successfull
  if (status==DictationSessionStatusSuccess){
	  DictionaryIterator *iter;
	  app_message_outbox_begin(&iter);
	  Tuplet value = TupletCString(1, transcription);
	  dict_write_tuplet(iter, &value);
	  app_message_outbox_send();
  } else {

  }
  dictation_session_stop(session);
  dictation_session_destroy(session);
}

static void start_dictation(){
  session = dictation_session_create(0, dictation_session_callback, NULL);
  dictation_session_enable_confirmation(session, true);
  dictation_session_enable_error_dialogs(session, true);
  dictation_session_start(session);	
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(text_layer, "Select");
  start_dictation();
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(text_layer, "Up");
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(text_layer, "Down");
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  text_layer = text_layer_create((GRect) { .origin = { 0, 72 }, .size = { bounds.size.w, 20 } });
  text_layer_set_text(text_layer, "Press a button");
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(text_layer));
}

static void window_unload(Window *window) {
  text_layer_destroy(text_layer);
}

static void init(void) {
  window = window_create();
  window_set_click_config_provider(window, click_config_provider);
  window_set_window_handlers(window, (WindowHandlers) {
	.load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  // setup app message
  window_stack_push(window, animated);
  app_message_register_outbox_sent(out_sent_handler);
  app_message_register_outbox_failed(out_failed_handler);
  const uint32_t inbound_size = 64;
  const uint32_t outbound_size = 64;
  app_message_open(inbound_size, outbound_size);
}

static void deinit(void) {
  window_destroy(window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}