#ifndef __BUTTONS_H__
#define __BUTTONS_H__

#include "mbed.h"
#include <vector>


typedef enum
{
    BtnNotPressed = 0,
    BtnPressed,
    BtnHold_50ms,
    BtnHold_1s,
    BtnHold_5s,
    BtnInvalid,
} ButtonState;



class Button
{

public:
    explicit Button(PinName pin_name);

    ButtonState get_state() const;

private:
    void raise_irq();
    void fall_irq();

private:
    InterruptIn pin;
    ButtonState last_state;
};


class Buttons
{

    static const int buttons_num = 3;

public:

    explicit Buttons();

    typedef enum
    {
        BtnOk= 0,
        BtnDown,
        BtnUp,
        BtnOther,
    } ButtonType;



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