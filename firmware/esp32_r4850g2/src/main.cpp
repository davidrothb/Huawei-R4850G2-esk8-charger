#include <Arduino.h>

#include <lvgl.h>
#include <Ticker.h>
#include <TFT_eSPI.h>
#include "TAMC_GT911.h"

#include "defines.h"


Ticker tick, scr_tick;  //ticker for keeping animations alive
TFT_eSPI tft = TFT_eSPI();  //screen driver
TAMC_GT911 tp = TAMC_GT911(TOUCH_SDA, TOUCH_SCL, TOUCH_INT, TOUCH_RST, SCREEN_WIDTH, SCREEN_HEIGHT); //i2c touch driver

static lv_disp_buf_t disp_buf;
static lv_color_t buf[LV_HOR_RES_MAX * 10];

static lv_style_t value_label_text_style, value_value_text_style;

lv_obj_t *scr_boot, 
         *scr_home, 
         *scr_param_num_set,
         *scr,
         *scr2;

lv_obj_t * slider_label;

bool switched = false;

lv_obj_t *screen1;
lv_obj_t *screen2;

void go_to_screen2(lv_event_t * e) {
  lv_scr_load(screen2);
}


static void lv_tick_handler(void)
{
  lv_tick_inc(LVGL_TICK_PERIOD);
}

static void go_to_screen2(lv_event_t * e)
{
  lv_scr_load(scr2);
}

static void scr_tick_handler(void){
  Serial.println("tick");
  if(switched){
    lv_scr_load(scr);
  }else{
    lv_scr_load(scr2);
  }
  switched = !switched;
}

/* Display flushing */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p){
  uint16_t c;
  tft.startWrite(); /* Start new TFT transaction */
  tft.setAddrWindow(area->x1, area->y1, (area->x2 - area->x1 + 1), (area->y2 - area->y1 + 1)); /* set the working window */
  for (int y = area->y1; y <= area->y2; y++) {
    for (int x = area->x1; x <= area->x2; x++) {
      c = color_p->full;
      tft.writeColor(c, 1);
      color_p++;
    }
  }
  tft.endWrite(); /* terminate TFT transaction */
  lv_disp_flush_ready(disp); /* tell lvgl that flushing is done */
}

bool captouch_read(lv_indev_drv_t * indev_driver, lv_indev_data_t * data){
    tp.read();
    if(!tp.isTouched || tp.points[0].size == 0){
      return false;
    }
    data->state = tp.isTouched ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL; 
    data->point.x = tp.points[0].x;
    data->point.y = tp.points[0].y;
    printf("Touch x: %i; y: %i\n", tp.points[0].x, tp.points[0].y);
    return false;
}

void printEvent(String Event, lv_event_t event)
{
  
  Serial.print(Event);
  printf(" ");

  switch(event) {
      case LV_EVENT_PRESSED:
          printf("Pressed\n");
          break;

      case LV_EVENT_SHORT_CLICKED:
          printf("Short clicked\n");
          break;

      case LV_EVENT_CLICKED:
          printf("Clicked\n");
          break;

      case LV_EVENT_LONG_PRESSED:
          printf("Long press\n");
          break;

      case LV_EVENT_LONG_PRESSED_REPEAT:
          printf("Long press repeat\n");
          break;

      case LV_EVENT_RELEASED:
          printf("Released\n");
          break;
  }
}






void slider_event_cb(lv_obj_t * slider, lv_event_t event)
{

  printEvent("Slider", event);

  if(event == LV_EVENT_VALUE_CHANGED) {
      static char buf[4];                                 /* max 3 bytes  for number plus 1 null terminating byte */
      snprintf(buf, 4, "%u", lv_slider_get_value(slider));
      lv_label_set_text(slider_label, buf);               /*Refresh the text*/
  }
}




void setup() {

  Serial.begin(115200);

  lv_init();

  tft.begin();
  tft.setRotation(3);

  tp.reset();
  tp.begin();
  tp.setRotation(ROTATION_LEFT);

  lv_disp_buf_init(&disp_buf, buf, NULL, LV_HOR_RES_MAX * 10);

  /*Initialize the display*/
  lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  disp_drv.hor_res = SCREEN_WIDTH;
  disp_drv.ver_res = SCREEN_HEIGHT;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.buffer = &disp_buf;
  lv_disp_drv_register(&disp_drv);

  lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);             /*Descriptor of a input device driver*/
  indev_drv.type = LV_INDEV_TYPE_POINTER;    /*Touch pad is a pointer-like device*/
  indev_drv.read_cb = captouch_read;      /*Set your driver function*/
  lv_indev_drv_register(&indev_drv);         /*Finally register the driver*/

  /*
  tick.attach_ms(LVGL_TICK_PERIOD, lv_tick_handler);

  //lv_obj_t * scr = lv_obj_create(NULL, NULL);
  scr = lv_obj_create(NULL, NULL);
  scr2 = lv_obj_create(NULL, NULL);

  lv_style_init(&value_label_text_style);
  lv_style_set_text_font(&value_label_text_style, LV_STATE_DEFAULT, LV_THEME_DEFAULT_FONT_SMALL);

  lv_style_init(&value_value_text_style);
  lv_style_set_text_font(&value_value_text_style, LV_STATE_DEFAULT, LV_THEME_DEFAULT_FONT_NORMAL);//LV_THEME_DEFAULT_FONT_BIG);

  lv_obj_t *label = lv_label_create(scr, NULL);
  lv_label_set_text(label, "Voltage out");
  lv_obj_align(label, NULL, LV_ALIGN_IN_TOP_LEFT, 10, 10);
  lv_obj_add_style(label, LV_LABEL_PART_MAIN, &value_label_text_style);

  lv_obj_t *label_curr = lv_label_create(scr, NULL);
  lv_label_set_text(label_curr, "50.40V");
  lv_obj_align(label_curr, NULL, LV_ALIGN_IN_TOP_LEFT, 10, 40);
  lv_obj_add_style(label_curr, LV_LABEL_PART_MAIN, &value_value_text_style);

  lv_obj_t *labe_curr_out = lv_label_create(scr, NULL);
  lv_label_set_text(labe_curr_out, "Current out");
  lv_obj_align(labe_curr_out, NULL, LV_ALIGN_IN_TOP_LEFT, 10, 110);
  lv_obj_add_style(labe_curr_out, LV_LABEL_PART_MAIN, &value_label_text_style);

  lv_obj_t *label_currr = lv_label_create(scr, NULL);
  lv_label_set_text(label_currr, "12.40A");
  lv_obj_align(label_currr, NULL, LV_ALIGN_IN_TOP_LEFT, 10, 140);
  lv_obj_add_style(label_currr, LV_LABEL_PART_MAIN, &value_value_text_style);

  lv_obj_t *label_pow_out = lv_label_create(scr, NULL);
  lv_label_set_text(label_pow_out, "Power out");
  lv_obj_align(label_pow_out, NULL, LV_ALIGN_IN_TOP_LEFT, 10, 210);
  lv_obj_add_style(label_pow_out, LV_LABEL_PART_MAIN, &value_label_text_style);

  lv_obj_t *label_power = lv_label_create(scr, NULL);
  lv_label_set_text(label_power, "1240W");
  lv_obj_align(label_power, NULL, LV_ALIGN_IN_TOP_LEFT, 10, 240);
  lv_obj_add_style(label_power, LV_LABEL_PART_MAIN, &value_value_text_style);

  //lv_disp_load_scr(scr);
  lv_scr_load(scr);

  lv_obj_t * slider = lv_slider_create(scr2, NULL);
    lv_obj_set_width(slider, SCREEN_WIDTH-50);                        
    lv_obj_set_height(slider, 50);
    lv_obj_align(slider, NULL, LV_ALIGN_CENTER, 0, 0);    
    lv_obj_set_event_cb(slider, slider_event_cb); 

    slider_label = lv_label_create(scr2, NULL);
    lv_label_set_text(slider_label, "0");
    lv_obj_set_auto_realign(slider, true);
    lv_obj_align(slider_label, slider, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
*/
 //////////////////////////////////////////////////////////   

  //lv_obj_t * spinner = lv_spinner_create(lv_scr_act(), NULL);
  //lv_obj_set_size(spinner, 100, 100);
  //lv_obj_align(spinner, NULL, LV_ALIGN_CENTER, 0, 0);

  /* lv_obj_t * tabview;
    tabview = lv_tabview_create(lv_scr_act(), NULL);

    lv_obj_t * tab1 = lv_tabview_add_tab(tabview, "Tab 1");
    lv_obj_t * tab2 = lv_tabview_add_tab(tabview, "Tab 2");
    lv_obj_t * tab3 = lv_tabview_add_tab(tabview, "Tab 3");

    lv_obj_t * label = lv_label_create(tab1, NULL);
    lv_label_set_text(label, "This the first tab\n\n"
                      "If the content\n"
                      "of a tab\n"
                      "becomes too\n"
                      "longer\n"
                      "than the\n"
                      "container\n"
                      "then it\n"
                      "automatically\n"
                      "becomes\n"
                      "scrollable.\n"
                      "\n"
                      "\n"
                      "\n"
                      "Can you see it?"
                      "\n"
                      "\n"
                      "Can you see it?"
                      
                      
                      );

    label = lv_label_create(tab2, NULL);
    lv_label_set_text(label, "Second tab");

    label = lv_label_create(tab3, NULL);
    lv_label_set_text(label, "Third tab"); */

    //lv_obj_scroll_to_view_recursive(label, LV_ANIM_ON);

    //scr_tick.attach_ms(3000, scr_tick_handler);

        // První obrazovka
        screen1 = lv_obj_create(NULL);
        lv_scr_load(screen1);
    
        // Tlačítko (obdélník) na první obrazovce
        lv_obj_t *btn = lv_btn_create(screen1);
        lv_obj_set_size(btn, 100, 50);
        lv_obj_align(btn, LV_ALIGN_CENTER, 0, 0);
        lv_obj_add_event_cb(btn, go_to_screen2, LV_EVENT_CLICKED, NULL);
    
        lv_obj_t *label = lv_label_create(btn);
        lv_label_set_text(label, "Další");
    
        // Druhá obrazovka
        screen2 = lv_obj_create(NULL);
        lv_obj_set_style_bg_color(screen2, lv_color_hex(0xFF0000), LV_PART_MAIN);  // Červené pozadí
    
        lv_obj_t *label2 = lv_label_create(screen2);
        lv_label_set_text(label2, "Jsi na druhé stránce");
        lv_obj_align(label2, LV_ALIGN_CENTER, 0, 0);
  
}


void loop() {
  lv_task_handler();

  /* if(!switched && ((esp_timer_get_time()/1000) > 5000)){
    Serial.println("Switch");
    //lv_obj_t * scr2 = lv_obj_create(NULL, NULL);
    lv_scr_load(scr2);
    switched = true;
  } */


  //uint64_t time = esp_timer_get_time() / 1000;
  //Serial.println(time);
/*   if((time%5000) < 10){
    Serial.println("5k trigger");
    //lv_obj_del(lv_scr_act());
    //lv_scr_load(scr2);
  }
  if((time%3000) < 10){
    Serial.println("3k trigger");
    //lv_obj_del(lv_scr_act());
    //lv_scr_load(scr);
  } */
  //delay(5);
}