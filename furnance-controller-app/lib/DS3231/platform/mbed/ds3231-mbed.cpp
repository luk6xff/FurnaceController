
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
void ds3231_mbed_init(I2C* i2c, uint8_t i2c_addr)
{
    _i2c = i2c;
    _i2c->frequency(400000);
    ds3231_init(i2c_addr);
}

//------------------------------------------------------------------------------
void ds3231_mbed_deinit(void)
{
    // Empty
}

//-----------------------------------------------------------------------------
bool ds3231_write(uint16_t addr, const uint8_t* buf, size_t buf_size)
{
    return _i2c->write((int)addr, (char*)buf, buf_size);
}

//-----------------------------------------------------------------------------
bool ds3231_read(uint16_t addr, uint8_t* buf, size_t buf_size)
{
    return _i2c->read(addr, (char*)buf, buf_size);
}
//-----------------------------------------------------------------------------


