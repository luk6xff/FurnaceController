
/**
 *  @brief:  Implementation of a TM1637 platform dependent [MBED] functions
 *  @author: luk6xff
 *  @email:  lukasz.uszko@gmail.com
 *  @date:   2019-12-06
 */

#include "tm1637-mbed.h"



/**
 * TM1637 pins
 */
DigitalInOut* _dio;
DigitalOut* _clk;

//------------------------------------------------------------------------------
void tm1637_mbed_init(tm1637* const dev, PinName dio, PinName clk)
{
    _dio = new DigitalInOut(dio);
    _clk = new DigitalOut(clk);
    tm1637_init(dev);
}

//------------------------------------------------------------------------------
void tm1637_mbed_deinit(void)
{
    // IO
    delete(_dio);
    delete(_clk);
}

//-----------------------------------------------------------------------------
void tm1637_delay_us(uint32_t us)
{
    wait_us(us);
}

//-----------------------------------------------------------------------------
void tm1637_set_dio_mode(tm1637_dio_mode mode)
{
    if (mode == TM1637_DIO_INPUT)
    {
        _dio->input();
    }
    else // mode == DIO_OUTPUT
    {
        _dio->output();
        //_dio.mode(PullUp);
    }
}

//-----------------------------------------------------------------------------
void tm1637_set_dio(tm1637_pin_state state)
{
    if (state == TM1637_PIN_HIGH)
    {
        *_dio = 1;
        return;
    }
    *_dio = 0;  // TM1637_PIN_LOW
}

//-----------------------------------------------------------------------------
tm1637_pin_state tm1637_get_dio()
{
    if (*_dio)
    {
        return TM1637_PIN_HIGH;
    }
    return TM1637_PIN_LOW;
}

//-----------------------------------------------------------------------------
void tm1637_set_clk(tm1637_pin_state state)
{
    if (state == TM1637_PIN_HIGH)
    {
        *_clk = 1;
        return;
    }
    *_clk = 0; // TM1637_PIN_LOW
}

//-----------------------------------------------------------------------------