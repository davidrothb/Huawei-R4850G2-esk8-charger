#include <MCUFRIEND_kbv.h>
#include <Adafruit_GFX.h>
#include <TouchScreen.h>
#include "defines.h"

#include <FreeDefaultFonts.h>
#include <Fonts/FreeMono12pt7b.h> //obecnej
#include <Fonts/FreeSansBold24pt7b.h> //davidrothb


MCUFRIEND_kbv tft;

//touchscreen variables
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
TSPoint tp;
uint16_t xpos, ypos;

//variables to keep track of screens displayed
int current_screen = SCREEN_OUT_SET, last_screen = SCREEN_BOOT;

//all the values used, first argument is the unit, second is the number of decimal places to print
value voltage_out("V", 2), current_out("A", 2), power_out("W", 0);
value voltage_out_set("V", 2), current_out_set("A", 2);
value voltage_out_default("V", 2), current_out_default("A", 2);

value voltage_in("V", 0), current_in("A", 1), power_in("W", 0), frequency_in("Hz", 0), efficiency_in("%", 0);

value charge_ah_in("Ah", 2), charge_wh_in("Wh", 0);

value temp_in("C", 0), temp_out("C", 0);


//checks if x & y are both in a specified range, used to filter out touches in a specific area
bool is_in_range(uint16_t& x, uint16_t& y, uint16_t x_min, uint16_t x_max, uint16_t y_min, uint16_t y_max){
  return (x >= x_min && x <= x_max && y >= y_min && y <= y_max);
}

void get_touch_coord(){
  tp = ts.getPoint();
  xpos = map(tp.y, TS_TOP, TS_BOT, 0, tft.width()) + TS_X_OFFSET;
  ypos = map(tp.x, TS_RT, TS_LEFT, 0, tft.height()) + TS_Y_OFFSET;
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
}

void render_screen(){
  switch(current_screen){
    case SCREEN_BOOT:
      //tft.setFont(&FreeSansBold24pt7b);
      tft.setTextSize(1);
      tft.setCursor(15, 264);
      tft.print("@davidrothb");
      //tft.setFont(&FreeMono12pt7b);
      tft.setCursor(20, 400);
      tft.print("Waiting for CAN init");
      Serial.println("BOOT_SCREEN base graphics rendered");
      break;

    case SCREEN_HOME:
      if(current_screen != last_screen){ //render base graphics if screens changed
          tft.fillScreen(BLACK);
          tft.setFont(NULL);
          tft.setTextSize(2);
          tft.setCursor(10, 10);
          tft.print("Voltage out");

          tft.setCursor(10, 110);
          tft.print("Current out");

          tft.setCursor(10, 210);
          tft.print("Power out");

          tft.drawLine(0, 300, 480, 300, WHITE);
          tft.fillRect(280, 0, 4, 300, WHITE);
          tft.fillRect(280, 100, 200, 4, WHITE);
          tft.fillRect(280, 205, 200, 4, WHITE);

          tft.setTextSize(2);
          tft.setCursor(295, 10);
          tft.print("Output SET"); 

          tft.setCursor(295, 115);
          tft.print("Input");  

          tft.setCursor(295, 220);
          tft.print("Charged");

          Serial.println("HOME_SCREEN base graphics rendered");
      }

      tft.setTextSize(7);
      if(voltage_out.has_changed()){
        tft.setCursor(10, 40);
        tft.print(voltage_out.print());
        Serial.println("HOME_SCREEN voltage_out rendered");
      }

      if(current_out.has_changed()){
        tft.setCursor(10, 140);
        tft.print(current_out.print());
        Serial.println("HOME_SCREEN current_out rendered");
      }

      if(power_out.has_changed()){
        tft.setCursor(10, 240);
        tft.print(power_out.print());
        Serial.println("HOME_SCREEN power_out rendered");
      }

      tft.setTextSize(3);
      if(voltage_out_set.has_changed()){
        tft.setCursor(300, 35);
        tft.print(voltage_out_set.print());
        Serial.println("HOME_SCREEN voltage_out_set rendered");
      }

      if(current_out_set.has_changed()){
        tft.setCursor(300, 65);
        tft.print(current_out_set.print());
        Serial.println("HOME_SCREEN current_out_set rendered");
      }

      if(voltage_in.has_changed() || current_in.has_changed()){
        tft.setCursor(300, 140);
        tft.print(voltage_in.print() + " " + current_in.print());
        Serial.println("HOME_SCREEN voltage_in & current_in rendered");
      }

      if(power_in.has_changed() || efficiency_in.has_changed()){
        tft.setCursor(300, 170);
        tft.print(power_in.print() + " " + efficiency_in.print());
        Serial.println("HOME_SCREEN power_in & efficiency_in rendered");
      }

      if(charge_ah_in.has_changed()){
        tft.setCursor(300, 245);
        tft.print(charge_ah_in.print());
        Serial.println("HOME_SCREEN charge_ah_in rendered");
      }

      if(charge_wh_in.has_changed()){
        tft.setCursor(300, 275);
        tft.print(charge_wh_in.print());
        Serial.println("HOME_SCREEN charge_wh_in rendered");
      }

      tft.setTextSize(2);
      if(temp_in.has_changed() || temp_out.has_changed()){
        tft.setCursor(0, 304);
        tft.print("T_in " + temp_in.print() + "|" + "T_out " + temp_out.print());
        Serial.println("HOME_SCREEN temp_in & temp_out rendered");
      }

      tft.setCursor(440, 304);
      tft.print("CAN");
      break;

    case SCREEN_OUT_SET:
      if(current_screen != last_screen){
        tft.fillScreen(BLACK);
        tft.setFont(NULL);
        tft.setCursor(10, 240);
        tft.fillRect(15, 10, 65, 65, GREY);
        tft.fillRect(400, 10, 65, 65, GREY);
        Serial.println("SCREEN_OUT_SET base graphics rendered");
      }

      tft.setTextSize(8);
      if(voltage_out_set.has_changed()){
        tft.setCursor(95, 15);
        tft.print(voltage_out_set.print());
        //Serial.println("SCREEN_OUT_SET voltage_out_set rendered");
      }
      


      break;
  }

  last_screen = current_screen;
}


void setup() {

    //init serial for debug
    Serial.begin(115200);

    //init the screen
    tft.reset();
    tft.begin(tft.readID()); 
    tft.setRotation(1);
    tft.fillScreen(BLACK);
    tft.setTextColor(WHITE, BLACK);

    voltage_out.current = 5.00;
    current_out.current = 6.12;
    power_out.current = 1256;

    voltage_out_set.current = 50.40;
    current_out_set.current = 12.15;

    voltage_in.current = 231;
    current_in.current = 0.432;
    power_in.current = 312;
    efficiency_in.current = 69;

    charge_ah_in.current = 10.32;
    charge_wh_in.current = 297;

    temp_in.current = 32;
    temp_out.current = 58;
  
  /* for(float f = 40.0; f < 99; f+=0.01){
    //napeti
    tft.setCursor(10, 40);
    tft.print(f);
    tft.print("V");

    //proud
    tft.setCursor(10, 140);
    tft.print(f-5.23);
    tft.print("A");

    tft.setCursor(10, 240);
    tft.print((int)(f*f));
    tft.print("W");
    
    delay(10);
  } */

}



void loop() {

  if(current_screen == SCREEN_BOOT && millis() >= BOOT_DURATION_MS) current_screen = SCREEN_HOME;

  get_touch_coord();

  /* if(!(millis() % 1000)){
    voltage_out.current+=0.16;
  } */

  voltage_out.current+=0.01;
  //current_out.current = voltage_out.current - 5;
  //power_out.current = voltage_out.current * current_out.current;
  
  if(tp.z > 0){
    tft.fillRect(xpos, ypos, 3, 3, RED);

    
    /* if(is_in_range(xpos, ypos, 0, 100, 0, 100)){
      tft.fillRect(xpos, ypos, 3, 3, BLUE);
    }else{
      tft.fillRect(xpos, ypos, 3, 3, RED);
    } */
    
    switch (current_screen){
      case SCREEN_HOME:
        if(is_in_range(xpos, ypos, 280, 480, 0, 100)){
          current_screen = SCREEN_OUT_SET;
        }
        break;

      case SCREEN_OUT_SET:
        if(is_in_range(xpos, ypos, 280, 480, 0, 100)){
          do{
            voltage_out_set.current += 0.1;
            render_screen();
            get_touch_coord();
          }while(tp.z > 0);
        }
        break;
    }

  }

  render_screen();

}