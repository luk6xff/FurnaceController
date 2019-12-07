#include "at24c64-mbed.h"



/**
 * I2C Interface
 */
static I2C* _i2c;

/**
 * AT24Cxx Write protection pin
 */
static DigitalOut* _wp;



//-----------------------------------------------------------------------------
void at24c64_mbed_init(PinName sda, PinName scl, PinName wp,
                      uint8_t chip_addr, size_t chip_size, size_t page_size)
{
    _i2c = new I2C(sda, scl);
    _wp = new DigitalOut(wp); 
    at24c64_init(chip_addr, chip_size, page_size);
}

//-----------------------------------------------------------------------------
void at24c64_mbed_deinit(void)
{
    at24c64_deinit();
}

//-----------------------------------------------------------------------------
AT24C64Status at24c64_io_init()
{
    // Set I2C frequency
    _i2c->frequency(400000);
}

//-----------------------------------------------------------------------------
AT24C64Status at24c64_io_deinit()
{
    // Empty
    return AT24C64_NOERR;
}

//-----------------------------------------------------------------------------
AT24C64Status at24c64_write_buffer(uint16_t addr, uint8_t* buf, size_t buf_size)
{
    // Check space
	if (!at24c64_check_space(addr, buf_size))
    {
        return AT24C64_OUT_OF_RANGE;
    }

    int ack = _i2c->write((int)addr, (char*)buf, buf_size);
    if (ack != 0)
    {
        return AT24C64_ERR;
    }
    return AT24C64_NOERR;
}

//-----------------------------------------------------------------------------
AT24C64Status at24c64_read_buffer(uint16_t addr, uint8_t* buf, size_t buf_size)
{
    int retVal = _i2c->read(addr, (char*)buf, buf_size);
    if (retVal != 0)
    {
        return AT24C64_ERR;
    }
    return AT24C64_NOERR;
}

//-----------------------------------------------------------------------------
void at24c64_enable_wp(bool enable)
{
    if (enable)
    {
        *_wp = 1;
        return;
    }
    *_wp = 0;
}


//-----------------------------------------------------------------------------
void at24c64_delay_ms(uint32_t delay_ms)
{
    wait_us(delay_ms*1000);
}

//-----------------------------------------------------------------------------

