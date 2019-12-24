#include "display.h"
#include "hw_config.h"


//------------------------------------------------------------------------------
Display::Display()
    : disp_mbed{DigitalInOut(TM1637_DIO), DigitalOut(TM1637_CLK)}
{
    tm1637_mbed_init(&disp, &disp_mbed);
}

//------------------------------------------------------------------------------
void Display::print(const char* data, size_t data_len, bool clear_on)
{
    if (clear_on)
    {
        clear();
    }
    tm1637_print(&disp, (const uint8_t*)data, data_len);
}

//------------------------------------------------------------------------------
void Display::clear()
{
    tm1637_clear(&disp);
}

//------------------------------------------------------------------------------
void Display::set_brightness(uint8_t brightness)
{
    tm1637_set_brightness(&disp, brightness);
}

//------------------------------------------------------------------------------
void Display::enable(bool on_off)
{
    tm1637_set_display(&disp, on_off);
}

//------------------------------------------------------------------------------
void Display::print_str(const char* str, size_t str_len, bool clear_on)
{
    if (clear_on)
    {
        clear();
    }
    tm1637_locate(&disp, 0);
    tm1637_set_brightness(&disp, TM1637_BRT5);
    tm1637_print(&disp, (const uint8_t*)str, str_len);
}

//------------------------------------------------------------------------------
void Display::print_dec(int dec, bool clear_on)
{
    const uint8_t num_of_chars = 4;
    char buf[num_of_chars];

    sprintf(buf, "%04d", dec);

    if (clear_on)
    {
        clear();
    }
    tm1637_locate(&disp, 0);
    tm1637_set_brightness(&disp, TM1637_BRT4);
    tm1637_print(&disp, (const uint8_t*)buf, num_of_chars);
}

//------------------------------------------------------------------------------
void Display::print_temperature(float temp, uint8_t brightness, bool clear_on)
{
    const uint8_t num_of_chars = 5;
    char buf[num_of_chars];
    int dec_temp = int(temp);

    if (dec_temp >= 0)
    {
        if (dec_temp < 10)
        {
            sprintf(buf, "%04.1f", temp);
            buf[4] = 1; // Degrees char
        }
        else if (dec_temp < 100)
        {
            sprintf(buf, "%2.1f", temp);
            buf[4] = 1; // Degrees char
        }
        else
        {
            sprintf(buf, "%d", dec_temp);
            buf[3] = 1; // Degrees char
        }
    }
    else
    {
        if (dec_temp < -10)
        {
            sprintf(buf, "%d", dec_temp);

            if (dec_temp > -100)
            {
                buf[3] = 1; // Degrees char
            }
        }
        else
        {
            snprintf(buf, num_of_chars, "%1.1f",temp);
            buf[4] = 1; // Degrees char
        }
    }

    if (clear_on)
    {
        clear();
    }
    tm1637_locate(&disp, 0);
    tm1637_set_brightness(&disp, brightness);
    tm1637_print(&disp, (const uint8_t*)buf, num_of_chars);
}

//------------------------------------------------------------------------------
void Display::print_time(uint8_t hour, uint8_t min, bool blink, uint8_t brightness)
{
    const uint8_t buf_size = 5;
    char buf[buf_size];

    if (blink)
    {
        sprintf(buf, "%02d.%02d", hour, min);
    }
    else
    {
        sprintf(buf, "%02d%02d", hour, min);
    }
    tm1637_locate(&disp, 0);
    tm1637_set_brightness(&disp, brightness);
    tm1637_print(&disp, (const uint8_t*)buf, buf_size);
}

//------------------------------------------------------------------------------
void Display::print_error(AppError error)
{
    tm1637_clear(&disp);
    tm1637_locate(&disp, 0);
    tm1637_set_brightness(&disp, TM1637_BRT5);
    tm1637_print(&disp, (const uint8_t*)app_errors[(int)error], 4);
}

//------------------------------------------------------------------------------