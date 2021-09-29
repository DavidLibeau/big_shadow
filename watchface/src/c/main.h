#include <pebble.h>
//vars

#define KEY_BASECOLOR 20
#define KEY_RSHADECOLOR 21
#define KEY_BSHADECOLOR 22
#define KEY_BACKCOLOR 23
#define KEY_SHOWSTATUS 1
#define KEY_BTVIBRATE 2
  
  
  
static int QTP_WINDOW_TIMEOUT = 2000;
static Window *qtp_window;
static bool qtp_is_showing;
static AppTimer *qtp_hide_timer;
// static InverterLayer *qtp_inverter_layer;


  
GFont font_block;  
Layer *sideDisplay;
GBitmap *siderastr;
int dd,mm,pw;
bool qtp_bluetooth_status;
  
Window *window;
Layer *grafika;
GBitmap  *rastr,*rastrd,*rastrh;  


// Methods
void drawNumber(GContext *ctx,int number,int x,int y, int size);

void qtp_setup();
void qtp_app_deinit();

void qtp_show();
void qtp_hide();
void qtp_timeout();

void qtp_tap_handler(AccelAxisType axis, int32_t direction);

void qtp_init();
void qtp_deinit();

 