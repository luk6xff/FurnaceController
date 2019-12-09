#include "buttons.h"
#include "hw_config.h"



//------------------------------------------------------------------------------
Buttons::Buttons()
    : btn_ok(BTN_OK)
    , btn_left(BTN_LEFT)
    , btn_right(BTN_RIGHT)
{
    btn_ok.mode(PullUp);
    btn_left.mode(PullUp);
    btn_right.mode(PullUp);
}

//------------------------------------------------------------------------------
Buttons::BtnInfo Buttons::check_buttons()
{
    BtnInfo info;
    info.state[BtnOk] = btn_ok.read() == 0 ? BtnPressed : BtnNotPressed;
    info.state[BtnLeft] = btn_left.read() == 0 ? BtnPressed : BtnNotPressed;
    info.state[BtnRight] = btn_right.read() == 0 ? BtnPressed : BtnNotPressed;
    return info;
}

//------------------------------------------------------------------------------