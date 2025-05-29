/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/


#include "lvgl.h"
#include "app_hal.h"

#include <cstdio>

#include "demos/lv_demos.h"



static lv_obj_t* label_voltage;
static lv_obj_t* label_current;
static lv_obj_t* label_power;
static lv_obj_t* main_screen;
static lv_obj_t* slider_screen;
static lv_obj_t* slider;
static lv_obj_t* slider_label;

static lv_obj_t* label_voltage_in;
static lv_obj_t* label_current_in;
static lv_obj_t* label_power_in;
static lv_obj_t* label_temp_in;

static lv_obj_t* label_efficiency_in;
static lv_obj_t* label_coulomb_in;
static lv_obj_t* slider_value_label;

static float voltage = 12.0;
static float current = 1.5;
static float power = 10.0;
static bool setting_voltage = true;

static void update_labels() {
  char buf[32];
  snprintf(buf, sizeof(buf), "voltage: %.2f V", voltage);
  lv_label_set_text(label_voltage, buf);

  snprintf(buf, sizeof(buf), "current: %.2f A", current);
  lv_label_set_text(label_current, buf);

  snprintf(buf, sizeof(buf), "power: %.2f W", power);
  lv_label_set_text(label_power, buf);
}

static void back_btn_event(lv_event_t* e) {
  int val = lv_slider_get_value(slider);
  switch ((uintptr_t)slider->user_data) {
    case 1: voltage = val / 10.0; break;                     // 0.1 V krok
    case 2: current = val / 100.0; break;                    // 0.01 A krok
    case 3: power   = val * 1.0; break;                      // 1 W krok
  }
  update_labels();
  lv_scr_load(main_screen);
}

static void slider_value_changed(lv_event_t* e) {
  int val = lv_slider_get_value(slider);
  char buf[16];
  switch ((uintptr_t)slider->user_data) {
    case 1: snprintf(buf, sizeof(buf), "%.1f V", val / 10.0); break;
    case 2: snprintf(buf, sizeof(buf), "%.2f A", val / 100.0); break;
    case 3: snprintf(buf, sizeof(buf), "%d W", val); break;
  }
  lv_label_set_text(slider_value_label, buf);
}

static void open_slider(lv_event_t* e) {
  uintptr_t index = (uintptr_t)e->user_data;
  slider->user_data = (void*)index;

  lv_scr_load(slider_screen);

  switch (index) {
    case 1:
      lv_label_set_text(slider_label, "voltage set (V)");
      lv_slider_set_range(slider, 100, 550); // 10.0–55.0 V
      lv_slider_set_value(slider, voltage * 10, LV_ANIM_OFF);
      break;
    case 2:
      lv_label_set_text(slider_label, "current set (A)");
      lv_slider_set_range(slider, 10, 5600); // 0.10–56.00 A
      lv_slider_set_value(slider, current * 100, LV_ANIM_OFF);
      break;
    case 3:
      lv_label_set_text(slider_label, "power set (W)");
      lv_slider_set_range(slider, 0, 3000); // 0–3000 W
      lv_slider_set_value(slider, power, LV_ANIM_OFF);
      break;
  }

  lv_obj_add_event_cb(slider, slider_value_changed, LV_EVENT_VALUE_CHANGED, NULL);
  slider_value_changed(NULL);
}


void ui_init() {
  main_screen = lv_obj_create(NULL);
  lv_obj_clear_flag(main_screen, LV_OBJ_FLAG_SCROLLABLE);

  // ========== LEVÝ SLOUPEC S TLAČÍTKY ==========
  lv_obj_t* btn_v = lv_btn_create(main_screen);
  lv_obj_set_size(btn_v, 150, 80);
  lv_obj_align(btn_v, LV_ALIGN_TOP_LEFT, 10, 10);
  label_voltage = lv_label_create(btn_v);
  lv_label_set_text(label_voltage, "voltage out: -- V");
  lv_obj_center(label_voltage);
  lv_obj_add_event_cb(btn_v, open_slider, LV_EVENT_CLICKED, (void*)1);

  lv_obj_t* btn_i = lv_btn_create(main_screen);
  lv_obj_set_size(btn_i, 150, 80);
  lv_obj_align_to(btn_i, btn_v, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 10);
  label_current = lv_label_create(btn_i);
  lv_label_set_text(label_current, "current out: -- A");
  lv_obj_center(label_current);
  lv_obj_add_event_cb(btn_i, open_slider, LV_EVENT_CLICKED, (void*)2);

  lv_obj_t* btn_p = lv_btn_create(main_screen);
  lv_obj_set_size(btn_p, 150, 80);
  lv_obj_align_to(btn_p, btn_i, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 10);
  label_power = lv_label_create(btn_p);
  lv_label_set_text(label_power, "power limit: -- W");
  lv_obj_center(label_power);
  lv_obj_add_event_cb(btn_p, open_slider, LV_EVENT_CLICKED, (void*)3);

  update_labels();

  // ========== PRAVÝ SLOUPEC S VÝSTUPY ==========
  label_voltage_in = lv_label_create(main_screen);
  lv_label_set_text(label_voltage_in, "voltage_in: 12.34 V");
  lv_obj_align(label_voltage_in, LV_ALIGN_TOP_RIGHT, -10, 10);

  label_current_in = lv_label_create(main_screen);
  lv_label_set_text(label_current_in, "current_in: 1.23 A");
  lv_obj_align_to(label_current_in, label_voltage_in, LV_ALIGN_OUT_BOTTOM_RIGHT, 0, 10);

  label_power_in = lv_label_create(main_screen);
  lv_label_set_text(label_power_in, "power_in: 15.2 W");
  lv_obj_align_to(label_power_in, label_current_in, LV_ALIGN_OUT_BOTTOM_RIGHT, 0, 10);

  label_temp_in = lv_label_create(main_screen);
  lv_label_set_text(label_temp_in, "temperature: 36.5 °C");
  lv_obj_align_to(label_temp_in, label_power_in, LV_ALIGN_OUT_BOTTOM_RIGHT, 0, 10);

  label_efficiency_in = lv_label_create(main_screen);
  lv_label_set_text(label_efficiency_in, "efficiency: 91.2 %");
  lv_obj_align_to(label_efficiency_in, label_temp_in, LV_ALIGN_OUT_BOTTOM_RIGHT, 0, 10);

  label_coulomb_in = lv_label_create(main_screen);
  lv_label_set_text(label_coulomb_in, "coulomb counter: 348 mAh");
  lv_obj_align_to(label_coulomb_in, label_efficiency_in, LV_ALIGN_OUT_BOTTOM_RIGHT, 0, 10);

  // ========== SLIDER obrazovka ==========
  slider_screen = lv_obj_create(NULL);
  lv_obj_clear_flag(slider_screen, LV_OBJ_FLAG_SCROLLABLE);

  slider_label = lv_label_create(slider_screen);
  lv_label_set_text(slider_label, "Nastavení");
  lv_obj_align(slider_label, LV_ALIGN_TOP_MID, 0, 20);

  slider = lv_slider_create(slider_screen);
  lv_obj_set_width(slider, 400);
  lv_obj_align(slider, LV_ALIGN_CENTER, 0, 0);

  slider_value_label = lv_label_create(slider_screen);
  lv_label_set_text(slider_value_label, "0.00");
  lv_obj_align_to(slider_value_label, slider, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

  lv_obj_add_event_cb(slider, slider_value_changed, LV_EVENT_VALUE_CHANGED, NULL);
  slider_value_changed(NULL);  // zobraz ihned aktuální hodnotu



  lv_obj_t* back_btn = lv_btn_create(slider_screen);
  lv_obj_set_size(back_btn, 100, 40);
  lv_obj_align(back_btn, LV_ALIGN_BOTTOM_MID, 0, -20);
  lv_obj_t* lbl = lv_label_create(back_btn);
  lv_label_set_text(lbl, "return");
  lv_obj_center(lbl);
  lv_obj_add_event_cb(back_btn, back_btn_event, LV_EVENT_CLICKED, NULL);

  lv_scr_load(main_screen);
}



#ifdef ARDUINO
#include <Arduino.h>

void setup() {
  lv_init();
  hal_setup();
  //lv_demo_widgets();
  //lv_demo_widgets();
  ui_init();  // naše GUI místo lv_demo_widgets
}

void loop() {
  hal_loop();
}

#else

int main(void)
{

	lv_init();

	hal_setup();

  //lv_demo_widgets();
  ui_init();  // naše GUI místo lv_demo_widgets

	hal_loop();
}

#endif /*ARDUINO*/