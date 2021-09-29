#include <pebble.h>
#include <math.h>

#include "main.h"

static char s_time_buffer[5]="0000";

#ifdef PBL_COLOR
static int colors[4]={0b11111111,0b11101010,0b11010101,0b11000000};
#else
static int colors[4]={255,213,213,192}; 
#endif  
TextLayer *t_debug,*t_day,*t_month,*t_accu,*t_bt;
bool showstatus=true,btvibrate=true,qtp_shown=false;
bool obstructed=false;
static char s_debug[]="data: 2222222"; 

int letter[][6][4]={
  {{00,00,00,13}, // 0
   {00,12,00,11}, 
   {00,11,00,11}, 
   {00,11,00,11}, 
   {00,00,00,11}, 
   {32,22,22,21}},
  {{33,00,13,33}, // 1
   {00,00,11,33}, 
   {32,00,11,33}, 
   {33,00,11,33}, 
   {00,00,00,13}, 
   {32,22,22,21}},
  {{00,00,00,13}, // 2
   {32,22,00,11}, 
   {00,00,00,11}, 
   {00,22,22,21}, 
   {00,00,00,13}, 
   {32,22,22,21}},
  {{00,00,00,13}, // 3
   {32,22,00,11}, 
   {33,00,00,11}, 
   {33,32,00,11}, 
   {00,00,00,11}, 
   {32,22,22,21}},
  {{00,13,00,13}, // 4
   {00,11,00,11}, 
   {00,00,00,11}, 
   {32,22,00,11}, 
   {33,33,00,11}, 
   {33,33,32,21}},
  {{00,00,00,13}, // 5
   {00,12,22,21}, 
   {00,00,00,13}, 
   {32,22,00,11}, 
   {00,00,00,11}, 
   {32,22,22,21}},
  {{00,00,00,13}, // 6
   {00,12,22,21}, 
   {00,00,00,13}, 
   {00,12,00,11}, 
   {00,00,00,11}, 
   {32,22,22,21}},
  {{00,00,00,13}, // 7
   {32,22,00,11}, 
   {33,33,00,11}, 
   {33,33,00,11}, 
   {33,33,00,11}, 
   {33,33,32,21}},
  {{00,00,00,13}, // 8
   {00,12,00,11}, 
   {00,00,00,11}, 
   {00,12,00,11}, 
   {00,00,00,11}, 
   {32,22,22,21}},
  {{00,00,00,13}, // 9
   {00,12,00,11}, 
   {00,00,00,11}, 
   {32,22,00,11}, 
   {00,00,00,11}, 
   {32,22,22,21}},
  {{00,00,00,13}, // T
   {32,00,12,21}, 
   {33,00,11,33}, 
   {33,00,11,33}, 
   {33,00,11,33}, 
   {33,32,21,33}},
};
  


void drawNumber(GContext *ctx,int number,int x,int y, int size) {
   GPath *s_horni_ptr = NULL;
   GPath *s_dolni_ptr = NULL;
   int i, j, kod, kodH, kodD;
  GPathInfo dolni = {.num_points = 3,
                      .points = (GPoint []) {{-1, -1}, {size-1, size-1}, {-1, size-1}}};
   GPathInfo horni = {.num_points = 3,
                      .points = (GPoint []) {{-1, 0}, {size, 0}, {size, size+1}}};
  
   s_horni_ptr = gpath_create(&horni);
   s_dolni_ptr = gpath_create(&dolni);
   graphics_context_set_compositing_mode(ctx,GCompOpAnd);

  for(i=0;i<6;i++) {
    for(j=0;j<4;j++) {
      kod = letter[number][i][j];
      kodD = floor(kod/10);
      kodH = kod-kodD*10;
      gpath_move_to(s_horni_ptr, GPoint(j*size+x, i*size+y));
      gpath_move_to(s_dolni_ptr, GPoint(j*size+x, i*size+y));
      
      // Fill the horni path:
      #if defined (PBL_COLOR)
        graphics_context_set_fill_color(ctx, (GColor8){ .argb = colors[kodH] });
      #else
        if (colors[kodH]>200) {
          graphics_context_set_fill_color(ctx, GColorWhite);
      } else {graphics_context_set_fill_color(ctx, GColorBlack);}
      #endif  

      gpath_draw_filled(ctx, s_horni_ptr);
      
      
      // Fill the dolni path:
      #if defined (PBL_COLOR)
        graphics_context_set_fill_color(ctx, (GColor8){ .argb = colors[kodD] });
      #else
        if (colors[kodD]>200) {
          graphics_context_set_fill_color(ctx, GColorWhite);
      } else {graphics_context_set_fill_color(ctx, GColorBlack);}
      #endif  
      gpath_draw_filled(ctx, s_dolni_ptr);

      #if defined (PBL_BW)
     graphics_context_set_compositing_mode(ctx,GCompOpAnd);
      if (colors[kodH]==213) {
        graphics_draw_bitmap_in_rect(ctx,rastrh,GRect(j*size+x,i*size+y,size,size));
      }
      if (colors[kodD]==213) {
        graphics_draw_bitmap_in_rect(ctx,rastrd,GRect(j*size+x,i*size+y,size,size));
      }
      #endif  
 
    }
  };
  gpath_destroy(s_horni_ptr);
  gpath_destroy(s_dolni_ptr);
}                         
  

static void prv_unobstructed_will_change(GRect final_unobstructed_screen_area, void *context) {
  // Get the full size of the screen
  GRect full_bounds = layer_get_bounds(window_get_root_layer(window));
  if (!grect_equal(&full_bounds, &final_unobstructed_screen_area)) {
    // Screen is about to become obstructed
    obstructed=true;
    layer_mark_dirty(grafika);
  }
}

static void prv_unobstructed_did_change(void *context) {
  // Get the full size of the screen
  GRect full_bounds = layer_get_bounds(window_get_root_layer(window));
  // Get the total available screen real-estate
  GRect bounds = layer_get_unobstructed_bounds(window_get_root_layer(window));
  if (grect_equal(&full_bounds, &bounds)) {
    // Screen is no longer obstructed
    obstructed=false;
    layer_mark_dirty(grafika);
  }
}


static void canvas_update_proc(Layer *this_layer, GContext *ctx) {
  int offX, offY;
  int size;
#if defined (PBL_COLOR)
  graphics_context_set_fill_color(ctx, (GColor8){ .argb = colors[3] });
#else
  if (colors[3]==0b11000000) {
      graphics_context_set_fill_color(ctx, GColorBlack);
  }  else {
      graphics_context_set_fill_color(ctx, GColorWhite);
  }
#endif  

  graphics_fill_rect(ctx, layer_get_bounds(this_layer), 0, GCornerNone);
#if defined (PBL_BW)
  if (colors[3]==213) graphics_draw_bitmap_in_rect(ctx,rastr,layer_get_bounds(this_layer));
#endif  

  if (obstructed) {
    size=PBL_IF_RECT_ELSE(layer_get_unobstructed_bounds(this_layer).size.h/12, 12);
    offY=0;
  } else {
    size=PBL_IF_RECT_ELSE(layer_get_bounds(this_layer).size.h/12, 12);
    offY=(layer_get_bounds(this_layer).size.h-12*size)/2;
  }
  offX=(layer_get_bounds(this_layer).size.w-8*size)/2;
  
  int h1 = s_time_buffer[0]-48;
  int h2 = s_time_buffer[1]-48;
  int m1 = s_time_buffer[2]-48;
  int m2 = s_time_buffer[3]-48;
  
//   h1=5;h2=6;
//   m1=7;m2=8;
  
  drawNumber(ctx, h1, offX, offY,size);
  drawNumber(ctx, h2, offX+4*size, offY,size);
  drawNumber(ctx, m1, offX, offY+6*size,size);
  drawNumber(ctx, m2, offX+4*size, offY+6*size,size);
}



static void updatetime() {
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  if (clock_is_24h_style()) {
    strftime(s_time_buffer, sizeof(s_time_buffer), "%H%M", tick_time);
  } else {
    strftime(s_time_buffer, sizeof(s_time_buffer), "%I%M", tick_time);
  }
  layer_mark_dirty(grafika);
//  if (!qtp_is_showing) {qtp_show();
//  qtp_is_showing = true;
//                       }
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  // Update things here
  updatetime();
}


static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  // Read first item
  Tuple *t = dict_read_first(iterator);

  // For all items
  while(t != NULL) {
    // Which key was received?
    switch(t->key) {
    case KEY_BASECOLOR:
      colors[0]= t->value->uint32;
      persist_write_int(KEY_BASECOLOR, colors[0]);
      break;
    case KEY_RSHADECOLOR:
      colors[1]= t->value->uint32;
      persist_write_int(KEY_RSHADECOLOR, colors[1]);
      break;
    case KEY_BSHADECOLOR:
      colors[2]= t->value->uint32;
      persist_write_int(KEY_BSHADECOLOR, colors[2]);
      break;
     case KEY_BACKCOLOR:
      colors[3]= t->value->uint32;
      persist_write_int(KEY_BACKCOLOR, colors[3]);
      break;     
     case KEY_BTVIBRATE:
      btvibrate=(t->value->int8==116)||(t->value->int8==84)||(t->value->int8==1);
      persist_write_int(KEY_BTVIBRATE, t->value->int8);
      break; 
     case KEY_SHOWSTATUS:
      showstatus=(t->value->int8==116)||(t->value->int8==84)||(t->value->int8==1);
//       showstatus=true;
      accel_tap_service_unsubscribe();
//       snprintf(s_debug,sizeof(s_debug),"data: %i",(int)t->value->int8);
//       text_layer_set_text(t_debug, s_debug);
      APP_LOG(APP_LOG_LEVEL_INFO, "%i", t->value->int8);
      if (showstatus) accel_tap_service_subscribe(&qtp_tap_handler);
      persist_write_int(KEY_SHOWSTATUS, t->value->int8);
      break; 
    default: 
      APP_LOG(APP_LOG_LEVEL_ERROR, "Key %d not recognized!", (int)t->key);
      break;
    }

    // Look for next item
    t = dict_read_next(iterator);
  }
//   APP_LOG(APP_LOG_LEVEL_INFO, "c:%i %i %i", colors[0],colors[1],colors[2]);
  
  updatetime();
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


void handle_init(void) {
 
  if (persist_exists(KEY_BSHADECOLOR)) {
    colors[0] = persist_read_int(KEY_BASECOLOR);
    colors[1] = persist_read_int(KEY_RSHADECOLOR);
    colors[2] = persist_read_int(KEY_BSHADECOLOR);
    colors[3] = persist_read_int(KEY_BACKCOLOR);
    showstatus = (persist_read_int(KEY_SHOWSTATUS)==116)||(persist_read_int(KEY_SHOWSTATUS)==84||(persist_read_int(KEY_SHOWSTATUS)==1));
    btvibrate = (persist_read_int(KEY_SHOWSTATUS)==116)||(persist_read_int(KEY_SHOWSTATUS)==84||(persist_read_int(KEY_SHOWSTATUS)==1));
    //showstatus=true;

    //     APP_LOG(APP_LOG_LEVEL_INFO, "Barva načtena! %d",colors[0]);
  } 
  qtp_setup();
  
	// Create a window and text layer
	window = window_create();
  grafika = layer_create(layer_get_bounds(window_get_root_layer(window)));
  rastr =gbitmap_create_with_resource(RESOURCE_ID_IMAGE_RASTR);
  rastrd =gbitmap_create_with_resource(RESOURCE_ID_IMG_RASTRD);
  rastrh =gbitmap_create_with_resource(RESOURCE_ID_IMG_RASTRH);

  // Set the text, font, and text alignment
	
	// Add the text layer to the window
	layer_add_child(window_get_root_layer(window), grafika);
  
  layer_set_update_proc(grafika, canvas_update_proc);

//   t_debug=text_layer_create(GRect(0,0,144,20));
// 	layer_add_child(window_get_root_layer(window), text_layer_get_layer(t_debug));
  
  
  //rastr = bitmap_layer_create(GRect(0, 0, 144, 168));
  //bitmap_layer_set_bitmap(rastr, resource)
  
	// Push the window
	window_stack_push(window, true);
  updatetime();
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);

   // Register callbacks
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);
   
  // Open AppMessage
  // APP_LOG(APP_LOG_LEVEL_INFO, "inbox max %i",app_message_inbox_size_maximum());
  // APP_LOG(APP_LOG_LEVEL_INFO, "outbox max %i",app_message_outbox_size_maximum());
  //app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
  app_message_open(250, 250);

    UnobstructedAreaHandlers handlers = {
    .will_change = prv_unobstructed_will_change,
    .did_change = prv_unobstructed_did_change
  };
  unobstructed_area_service_subscribe(handlers, NULL);

  
 //app_timer_register(1000, qtp_show, NULL);
}



void handle_deinit(void) {
	// Destroy the layer
	layer_destroy(grafika);
  gbitmap_destroy(rastr);
  gbitmap_destroy(rastrd);
  gbitmap_destroy(rastrh);
	
	// Destroy the window
	window_destroy(window);
  qtp_app_deinit();
}

int main(void) {
	handle_init();
	app_event_loop();
	handle_deinit();
}

////////////////////////////////////////////////////////////////////////////
//sidedisplay stuff
////////////////////////////////////////////////////////////////////////////
void qtp_bluetooth_callback(bool connected) {

  if (btvibrate) {
    if (connected) {
  			vibes_short_pulse();
  		} else {
  			vibes_double_pulse();
  		}
  }    
}


void qtp_setup() {
	qtp_is_showing = false;
	qtp_shown = false;
	if (showstatus) accel_tap_service_subscribe(&qtp_tap_handler);
  font_block = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_BLOCK_20));
	qtp_bluetooth_status = bluetooth_connection_service_peek();
	bluetooth_connection_service_subscribe(qtp_bluetooth_callback);
}

/* Handle taps from the hardware */
void qtp_tap_handler(AccelAxisType axis, int32_t direction) {
	if (qtp_shown) {
		qtp_hide();
	} else if (!qtp_is_showing) {
    //tady přidáme načtení data a napájení
    qtp_is_showing=true;
    app_timer_register(1000, qtp_show, NULL);
	}
}

static void side_update_proc(Layer *this_layer, GContext *ctx) {
  int offX, offY,d1,d2,bat, moffX,moffY,size;

  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);
  char chTime[10] = "";
  
  
// vzplnime pozadi barvou pozadi
#ifdef PBL_COLOR 
  graphics_context_set_fill_color(ctx, (GColor8){ .argb = colors[3] });
#else
    if (colors[3]==0b11000000) {
          graphics_context_set_fill_color(ctx, GColorBlack);
    } else {graphics_context_set_fill_color(ctx, GColorWhite);}
#endif  

  graphics_fill_rect(ctx, layer_get_bounds(this_layer), 0, GCornerNone);

// Jestli je pozadi na BW rastr, musime ho vyplnit  
#ifndef PBL_COLOR   
    if (colors[3]==213) {
    graphics_context_set_compositing_mode(ctx,GCompOpAnd);
    graphics_draw_bitmap_in_rect(ctx,rastr,layer_get_bounds(this_layer));
  }
#endif
  
  
//horni a dolni lista s popisky  
#ifdef PBL_COLOR 
  text_layer_set_text_color(t_day,(GColor8){ .argb = colors[0]});
  text_layer_set_text_color(t_month,(GColor8){ .argb = colors[0]});
  text_layer_set_text_color(t_accu,(GColor8){ .argb = colors[0]});
  text_layer_set_text_color(t_bt,(GColor8){ .argb = colors[0]});

#else
   if (colors[3]>200) {
    text_layer_set_text_color(t_day,GColorBlack);
    text_layer_set_text_color(t_month,GColorBlack);
    text_layer_set_text_color(t_accu,GColorBlack);
    text_layer_set_text_color(t_bt,GColorBlack);
   } else {
    text_layer_set_text_color(t_day,GColorWhite);
    text_layer_set_text_color(t_month,GColorWhite);
    text_layer_set_text_color(t_accu,GColorWhite);
    text_layer_set_text_color(t_bt,GColorWhite);
   } 
 #endif  

  moffX=(layer_get_bounds(this_layer).size.w-120)/2;
  moffY=(layer_get_bounds(this_layer).size.h-128)/2;;
  
  size=6; 
  //den
  strftime(chTime, sizeof(chTime), "%d", tick_time);
  d1 = chTime[0]-48;
  d2 = chTime[1]-48;
  offX=moffX;
  offY=moffY;
  #if defined(PBL_ROUND)
  offY=moffY+8;
  #endif
  drawNumber(ctx, d1, offX, offY,size);
  drawNumber(ctx, d2, offX+24, offY,size);

  //měsíc
  strftime(chTime, sizeof(chTime), "%m", tick_time);
  d1 = chTime[0]-48;
  d2 = chTime[1]-48;
  offX=moffX + 72;
  drawNumber(ctx, d1, offX, offY,size);
  drawNumber(ctx, d2, offX+24, offY,size);

  //baterka
	BatteryChargeState charge_state = battery_state_service_peek();
  bat=charge_state.charge_percent;
  if (bat>90) {bat=99;}
  snprintf(chTime, sizeof(chTime), "%d", bat);
  
  d1 = chTime[0]-48;
  d2 = chTime[1]-48;
  offX=moffX;
  offY=moffY+84;
  drawNumber(ctx, d1, offX, offY,size);
  drawNumber(ctx, d2, offX+24, offY,size);

  
  if (bluetooth_connection_service_peek()) {
    offX=moffX+72;
    offY=moffY+84;
    drawNumber(ctx, 8, offX, offY,size);
    drawNumber(ctx, 10, offX+24, offY,size);
  	} 
  
}


/* Create the QTPlus Window and initialize the layres */
void qtp_init() {
  int width, mx;
	qtp_window = window_create();
  width=layer_get_bounds(window_get_root_layer(qtp_window)).size.w;
  //height=layer_get_bounds(window_get_root_layer(qtp_window)).size.h;
  siderastr = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_POPISKY_BLACK);

  #if defined (PBL_ROUND)
    t_day=text_layer_create(GRect(0,4,width,25));
    text_layer_set_text_alignment(t_day, GTextAlignmentCenter);
    text_layer_set_text(t_day,"DATE");
  #else
    t_day=text_layer_create(GRect(11,-4,70,25));
    text_layer_set_text(t_day,"DAY");
  #endif
  
    mx=(width-122)/2;
  
    text_layer_set_font(t_day,font_block);
    text_layer_set_background_color(t_day,GColorClear);
  
    t_accu=text_layer_create(GRect(mx,80,70,25));
    text_layer_set_text(t_accu,"CHRG");
    text_layer_set_font(t_accu,font_block);
    text_layer_set_background_color(t_accu,GColorClear);

    t_month=text_layer_create(GRect(mx+72,-4,70,25));
    text_layer_set_text(t_month,"MONTH");
    text_layer_set_font(t_month,font_block);
    text_layer_set_background_color(t_month,GColorClear);
    
    t_bt=text_layer_create(GRect(mx+72,80,70,25));
    text_layer_set_text(t_bt,"CONN");
    text_layer_set_font(t_bt,font_block);
    text_layer_set_background_color(t_bt,GColorClear);


  
    sideDisplay=layer_create(layer_get_bounds(window_get_root_layer(qtp_window)));
    layer_set_update_proc(sideDisplay, side_update_proc);
    layer_add_child(window_get_root_layer(qtp_window), sideDisplay);
    layer_add_child(window_get_root_layer(qtp_window), text_layer_get_layer(t_day));
    #if defined (PBL_RECT)
      layer_add_child(window_get_root_layer(qtp_window), text_layer_get_layer(t_month)); 
    #endif 
    layer_add_child(window_get_root_layer(qtp_window), text_layer_get_layer(t_accu));
    layer_add_child(window_get_root_layer(qtp_window), text_layer_get_layer(t_bt));
  
}


/* Deallocate QTPlus items when window is hidden */
void qtp_deinit() {
  layer_destroy(sideDisplay);
  text_layer_destroy(t_day);
  text_layer_destroy(t_month);
  text_layer_destroy(t_accu);
  text_layer_destroy(t_bt);
  gbitmap_destroy(siderastr);
  window_destroy(qtp_window);
	app_timer_cancel(qtp_hide_timer);
}

/* Deallocate persistent QTPlus items when watchface exits */
void qtp_app_deinit() {
		bluetooth_connection_service_unsubscribe();
    accel_tap_service_unsubscribe();
}

/* Create window, layers, text. Display QTPlus */
void qtp_show() {
	qtp_init();
  //APP_LOG(APP_LOG_LEVEL_INFO, "show stat");
  qtp_is_showing=false;
  qtp_shown=true;
  window_stack_push(qtp_window, true);
	qtp_hide_timer = app_timer_register(QTP_WINDOW_TIMEOUT, qtp_hide, NULL);
}

/* Hide QTPlus. Free memory */
void qtp_hide() {
  //APP_LOG(APP_LOG_LEVEL_INFO, "hide stat");
	window_stack_pop(true);
  qtp_shown=false;
  qtp_deinit();
}

