#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include "../lib/TM1637/platform/mbed/tm1637-mbed.h"

class Display
{
public:
    Display();
    void print(const char* data, size_t data_len);
    void clear();
    void set_brightness(uint8_t brightness);
    void enable(bool on_off);

    void print_temperature(float temp);
    void print_time(uint8_t hour, uint8_t min, bool blink = false);

private:
    tm1637 disp;
    tm1637_mbed disp_mbed;
};


#endif // __DISPLAY_H__
