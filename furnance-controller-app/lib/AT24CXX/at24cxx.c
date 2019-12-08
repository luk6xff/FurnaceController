 #include "at24cxx.h"


//------------------------------------------------------------------------------
static const at24cxx_mem at24cxx_devices[] =
{
    { AT24C01,   128,    8   },  //128 bytes=1Kbit, 8bytes=64bit
    { AT24C02,   256,    8   },
    { AT24C04,   512,    16  },
    { AT24C08,   1024,   16  },
    { AT24C16,   2048,   16  },
    { AT24C32,   4096,   32  },
    { AT24CXX,   8192,   32  },
    { AT24C128,  16384,  64  },
    { AT24C256,  32768,  64  },
    { AT24C512,  65536,  128 },
    { AT24C1024, 131072, 128 },
};


//------------------------------------------------------------------------------
at24cxx_status at24cxx_init(at24cxx* const dev)
{
    if (!dev)
    {
        return AT24CXX_ERR;
    }
    uint8_t i2c_addr = dev->addr;
    switch (dev->type)
    {
        case AT24C01:
        case AT24C02:
            if (i2c_addr > 0x07)
            {
                return AT24CXX_BAD_ADDR;
            }
            dev->addr = AT24CXX_ADDR | (i2c_addr << 1);
            break;

        case AT24C04:
            if (i2c_addr > 0x07)
            {
                return AT24CXX_BAD_ADDR;
            }
            dev->addr = AT24CXX_ADDR | (i2c_addr & 0xFE) << 1;
            break;

        case AT24C08:
            if (i2c_addr > 0x07)
            {
                return AT24CXX_BAD_ADDR;
            }
            dev->addr = AT24CXX_ADDR | (i2c_addr & 0xFC) << 1;
            break;

        case AT24C16:
            dev->addr = AT24CXX_ADDR;
            break;

        case AT24C32:
        case AT24CXX:
            if (i2c_addr > 0x07)
            {
                return AT24CXX_BAD_ADDR;
            }
            dev->addr = AT24CXX_ADDR | (i2c_addr << 1);
            break;

        case AT24C128:
        case AT24C256:
        case AT24C512:
            if (i2c_addr > 0x03)
            {
                return AT24CXX_BAD_ADDR;
            }
            dev->addr = AT24CXX_ADDR | (i2c_addr << 1);
            break;

        case AT24C1024:
            if (i2c_addr > 0x07)
            {
                return AT24CXX_BAD_ADDR;
            }
            dev->addr = AT24CXX_ADDR | (i2c_addr & 0xFE) << 1;
            break;

        default:
            return AT24CXX_PARAM_ERR;
    }
    at24cxx_io_init(dev);

    return AT24CXX_NOERR;
}


//------------------------------------------------------------------------------
// at24cxx_status at24cxx_write(const at24cxx* const dev, uint32_t addr,
//                              uint8_t* data, uint32_t data_size)
// {
//     at24cxx_status ret = AT24CXX_NOERR;
//     uint8_t page;
//     uint8_t addr = 0;
//     uint8_t remain;
//     uint8_t fpart,lpart;
//     uint8_t i,j,ind;
//     uint8_t cmd[129];

//     if (!at24cxx_check_space(dev, addr, data_size))
//     {
//         return AT24CXX_OUT_OF_RANGE;
//     }


//     // Number of blocks of page size to be written.
//     size_t blocks_num       = data_size / at24cxx_mem[dev->type].page_size;
//     // Number of remaining bytes to be written.
//     size_t remain_bytes_num = data_size % at24cxx_mem[dev->type].page_size;

//     for (size_t i = 0; i < blocks_num; i++)
//     {
//         uint8_t offset = i * AT24CXX_page_size;
//         ret = at24cxx_write_buffer(addr + offset, buf + offset, AT24CXX_page_size);
//         if (ret != AT24CXX_NOERR)
//         {
//             return ret;
//         }
//     }

// }


////-----------------------------------------------------------------------------
//at24cxx_status at24cxx_write_bytes(uint16_t addr, uint8_t data)
//{
//    return at24cxx_write_buffer(addr, &data, 1);
//}
//
////-----------------------------------------------------------------------------
//at24cxx_status at24cxx_read_byte(uint16_t addr, uint8_t* data)
//{
//    at24cxx_status ret = AT24CXX_ERR;
//    if (data)
//    {
//        ret = at24cxx_read_buffer(addr, data, 1);
//    }
//    return ret;
//}
//
////-----------------------------------------------------------------------------
//at24cxx_status at24cxx_write_page(uint16_t addr, uint8_t* buf, size_t buf_size)
//{
//    at24cxx_status ret = AT24CXX_ERR;
//    // Write full page buffers.
//    size_t bufferCount = buf_size / AT24CXX_page_size;
//    for (size_t i = 0; i < bufferCount; i++)
//    {
//        uint8_t offset = i * AT24CXX_page_size;
//        ret = at24cxx_write_buffer(addr + offset, buf + offset, AT24CXX_page_size);
//        if (ret != AT24CXX_NOERR)
//        {
//            return ret;
//        }
//    }
//
//    // Write remaining bytes.
//    uint8_t remainingBytes = buf_size % AT24CXX_page_size;
//    uint8_t offset = buf_size - remainingBytes;
//    return at24cxx_write_buffer(addr + offset, buf + offset, remainingBytes);
//}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool at24cxx_check_space(const at24cxx* const dev, uint16_t addr, size_t size)
{
    if (dev->type < AT24CINVALID)
    {
        // Check that the address start in the chip and doesn't extend too broad
        if ((addr >= at24cxx_devices[dev->type].size) ||
           ((addr + size) >= at24cxx_devices[dev->type].size))
        {
            return false;
        }
        else
        {
            return true;
        }
    }
    return false;
}

