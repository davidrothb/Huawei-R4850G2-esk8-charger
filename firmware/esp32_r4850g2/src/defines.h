#include <Arduino.h>

//colors
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
#define GREY    0x8410

#define BOOT_DURATION_MS 1000
#define LVGL_TICK_PERIOD 10

//touchscreen constants
#define TOUCH_SDA  33
#define TOUCH_SCL  32
#define TOUCH_INT 21
#define TOUCH_RST 25
#define SCREEN_WIDTH  480
#define SCREEN_HEIGHT 320

//fancy names for different screens
enum screen{
    SCREEN_BOOT,
    SCREEN_HOME,
    SCREEN_OUT_SET,
    SCREEN_IN_SET
};

//this helps keep track of whether a value has changed along with printing it with its unit
struct value{
    float current;
    float last;

    String unit;
    int decimal_places;

    value(String unit, int decimal_places) : unit(unit), decimal_places(decimal_places){};
    bool has_changed(){return current != last;}
    String print(){
        last = current;
        return (current<10?"0":"") + String(current, decimal_places) + unit;
    }
};

