
/**
 *  @brief:  Implementation of a DS321 platform dependent [MBED] functions
 *  @author: luk6xff
 *  @email:  lukasz.uszko@gmail.com
 *  @date:   2019-11-15
 */

#include "ds3231-mbed.h"

/**
 * I2C Interface
 */
static I2C* _i2c;

//------------------------------------------------------------------------------
void ds3231_mbed_init(PinName sda, PinName scl)
{
    _i2c = new I2C(sda, scl);
    _i2c->frequency(400000);
    ds3231_init(DS3231_I2C_ADRS);
}

//------------------------------------------------------------------------------
void ds3231_mbed_deinit(void)
{
    // IO
    delete(_i2c);
}

//-----------------------------------------------------------------------------
bool ds3231_write_buffer(uint16_t addr, uint8_t* buf, size_t buf_size)
{
    int ret = _i2c->write((int)addr, (char*)buf, buf_size);
    if (ret != 0)
    {
        return false;
    }
    return true;
}

//-----------------------------------------------------------------------------
bool ds3231_read_buffer(uint16_t addr, uint8_t* buf, size_t buf_size)
{
    int ret = _i2c->read(addr, (char*)buf, buf_size);
    if (ret != 0)
    {
        return false;
    }
    return true;
}
//-----------------------------------------------------------------------------

