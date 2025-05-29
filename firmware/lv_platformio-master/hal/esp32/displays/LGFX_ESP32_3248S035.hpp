#pragma once

#define LGFX_USE_V1
#include <LovyanGFX.hpp>

#define WIDTH 480
#define HEIGHT 320

class LGFX : public lgfx::LGFX_Device
{
    lgfx::Panel_ST7796   _panel_instance;
    lgfx::Bus_SPI        _bus_instance;
    lgfx::Light_PWM      _light_instance;
    //lgfx::Touch_GT911    _touch_instance;  // změněno z XPT2046 na GT911

public:
    LGFX(void)
    {
        {   // SPI bus config
            auto cfg = _bus_instance.config();

            cfg.spi_host   = VSPI_HOST;  // HSPI_HOST nebo VSPI_HOST
            cfg.spi_mode   = 3;          // SPI mode (0-3)
            cfg.freq_write = 40000000;   // SPI write speed
            cfg.freq_read  = 16000000;   // SPI read speed
            cfg.spi_3wire  = false;
            cfg.use_lock   = true;
            cfg.dma_channel = 1;

            cfg.pin_sclk = 14;
            cfg.pin_mosi = 13;
            cfg.pin_miso = 12;
            cfg.pin_dc   = 2;

            _bus_instance.config(cfg);
            _panel_instance.setBus(&_bus_instance);
        }

        {   // ST7796 panel config
            auto cfg = _panel_instance.config();

            cfg.pin_cs     = 15;
            cfg.pin_rst    = -1;
            cfg.pin_busy   = -1;

            cfg.memory_width  = 320;
            cfg.memory_height = 480;
            cfg.panel_width   = 320;
            cfg.panel_height  = 480;

            cfg.offset_x = 0;
            cfg.offset_y = 0;
            cfg.offset_rotation = 0;

            cfg.dummy_read_pixel = 8;
            cfg.dummy_read_bits  = 1;
            cfg.readable   = false;
            cfg.invert     = false;
            cfg.rgb_order  = false;
            cfg.dlen_16bit = false;
            cfg.bus_shared = true;

            _panel_instance.config(cfg);
        }

        {   // Backlight config
            auto cfg = _light_instance.config();

            cfg.pin_bl      = 27;
            cfg.invert      = false;
            cfg.freq        = 44100;
            cfg.pwm_channel = 0;

            _light_instance.config(cfg);
            _panel_instance.setLight(&_light_instance);
        }
        /*
        {   // Touch GT911 config
            auto cfg = _touch_instance.config();

            cfg.bus_type = lgfx::ITouch::bus_type_t::i2c;

            cfg.pin_sda = 33;
            cfg.pin_scl = 32;
            cfg.pin_int = 21;
            cfg.pin_rst = 25;

            //cfg.bus_type = lgfx::Bus_I2C::bus_type_t::i2c;  // I2C sběrnice
            cfg.i2c_port = 1;  // I2C port 0 nebo 1 podle tvého zapojení
            cfg.i2c_addr = 0x5D; // typická adresa GT911, může se lišit, ověř si

            cfg.freq = 400000;  // I2C frekvence

            cfg.x_min = 0;
            cfg.x_max = 319;
            cfg.y_min = 0;
            cfg.y_max = 479;
            cfg.offset_rotation = 0;

            _touch_instance.config(cfg);
            _panel_instance.setTouch(&_touch_instance);
        }
        */

        setPanel(&_panel_instance);
    }
};

LGFX tft;
