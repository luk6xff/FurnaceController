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

private:
    tm1637 disp;
};


#endif // __DISPLAY_H__
