#ifndef __BUTTONS_H__
#define __BUTTONS_H__

#include "mbed.h"
#include <vector>

class Buttons
{

    static const int buttons_num = 3;

public:

    explicit Buttons();

    typedef enum
    {
        BtnOk= 0,
        BtnLeft,
        BtnRight,
        BtnOther,
    } ButtonType;

    typedef enum
    {
        BtnNotPressed = 0,
        BtnPressed,
        BtnHold,
        BtnInvalid,
    } ButtonState;

    typedef struct
    {
        ButtonState state[buttons_num];
    } BtnInfo;


    BtnInfo check_buttons();

private:
    DigitalIn btn_ok;
    DigitalIn btn_left;
    DigitalIn btn_right;
};

#endif // __BUTTONS_H__