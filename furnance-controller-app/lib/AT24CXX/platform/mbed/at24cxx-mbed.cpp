#include "at24cxx-mbed.h"




//-----------------------------------------------------------------------------
void at24cxx_mbed_init(at24cxx* const dev, at24cxx_mbed* const mbed_dev)
{
    dev->platform_dev = mbed_dev;
    at24cxx_init(dev);
}

//-----------------------------------------------------------------------------
void at24cxx_mbed_deinit(at24cxx* const dev)
{
    at24cxx_io_deinit(dev);
}

//-----------------------------------------------------------------------------
at24cxx_status at24cxx_io_init(at24cxx* const dev)
{
    at24cxx_mbed* pd = (at24cxx_mbed*)dev->platform_dev;
    // Set I2C frequency
    pd->i2c->frequency(400000);
    return AT24CXX_NOERR;
}

//-----------------------------------------------------------------------------
at24cxx_status at24cxx_io_deinit(at24cxx* const dev)
{
    // Empty
    return AT24CXX_NOERR;
}

// //-----------------------------------------------------------------------------
// at24cxx_status at24cxx_write_buffer(uint16_t addr, uint8_t* buf, size_t buf_size)
// {
//     // Check space
// 	if (!at24cxx_check_space(addr, buf_size))
//     {
//         return AT24CXX_OUT_OF_RANGE;
//     }

//     int ack = _i2c->write((int)addr, (char*)buf, buf_size);
//     if (ack != 0)
//     {
//         return AT24CXX_ERR;
//     }
//     return AT24CXX_NOERR;
// }

// //-----------------------------------------------------------------------------
// at24cxx_status at24cxx_read_buffer(uint16_t addr, uint8_t* buf, size_t buf_size)
// {
//     int retVal = _i2c->read(addr, (char*)buf, buf_size);
//     if (retVal != 0)
//     {
//         return AT24CXX_ERR;
//     }
//     return AT24CXX_NOERR;
// }

//-----------------------------------------------------------------------------
void at24cxx_enable_wp(at24cxx* const dev, bool enable)
{
    at24cxx_mbed* pd = (at24cxx_mbed*)dev->platform_dev;
    if (enable)
    {
        *(pd->wp) = 1;
        return;
    }
    *(pd->wp) = 0;
}


//-----------------------------------------------------------------------------
void at24cxx_delay_ms(uint32_t delay_ms)
{
    wait_us(delay_ms*1000);
}

//-----------------------------------------------------------------------------

