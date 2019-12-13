#ifndef __BUTTONS_H__
#define __BUTTONS_H__

#include "mbed.h"
#include "button.h"
#include <map>


typedef enum
{
    BtnTypeOk= 0,
    BtnTypeDown,
    BtnTypeUp,
} ButtonType;

class Buttons
{

    static const int buttons_num = 3;

public:

    explicit Buttons();

    ButtonState check_button(ButtonType type);

private:
    std::map<ButtonType, Button*> buttons_map;

    // Buttons
    Button btn_ok;
    Button btn_down;
    Button btn_up;
};

#endif // __BUTTONS_H__