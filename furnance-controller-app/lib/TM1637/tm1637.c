#include "tm1637.h"

//------------------------------------------------------------------------------
// @brief GLOBAL VARIABLES
//------------------------------------------------------------------------------
//Mask for blending out and restoring Icons
const char MASK_ICON_GRID[] = {
                                LO(S7_ICON_GR1),
                                LO(S7_ICON_GR2),
                                LO(S7_ICON_GR3),
                                LO(S7_ICON_GR4),
                                LO(S7_ICON_GR5),
                                LO(S7_ICON_GR6)
                              };

// ASCII Font definition table for transmission to TM1637

// Display all ASCII characters
const short FONT_7S[]  = {
                             C7_SPC, //32 0x20, Space
                             C7_EXC,
                             C7_QTE,
                             C7_HSH,
                             C7_DLR,
                             C7_PCT,
                             C7_AMP,
                             C7_ACC,
                             C7_LBR,
                             C7_RBR,
                             C7_MLT,
                             C7_PLS,
                             C7_CMA,
                             C7_MIN,
                             C7_DPT,
                             C7_RS,
                             C7_0,   //48 0x30
                             C7_1,
                             C7_2,
                             C7_3,
                             C7_4,
                             C7_5,
                             C7_6,
                             C7_7,
                             C7_8,
                             C7_9,
                             C7_COL, //58 0x3A
                             C7_SCL,
                             C7_LT,
                             C7_EQ,
                             C7_GT,
                             C7_QM,
                             C7_AT,  //64 0x40
                             C7_A,   //65 0x41, A
                             C7_B,
                             C7_C,
                             C7_D,
                             C7_E,
                             C7_F,
                             C7_G,
                             C7_H,
                             C7_I,
                             C7_J,
                             C7_K,
                             C7_L,
                             C7_M,
                             C7_N,
                             C7_O,
                             C7_P,
                             C7_Q,
                             C7_R,
                             C7_S,
                             C7_T,
                             C7_U,
                             C7_V,
                             C7_W,
                             C7_X,
                             C7_Y,
                             C7_Z,   //90 0x5A, Z
                             C7_SBL, //91 0x5B
                             C7_LS,
                             C7_SBR,
                             C7_PWR,
                             C7_UDS,
                             C7_ACC,
                             C7_A,   //97 0x61, A replacing a
                             C7_B,
                             C7_C,
                             C7_D,
                             C7_E,
                             C7_F,
                             C7_G,
                             C7_H,
                             C7_I,
                             C7_J,
                             C7_K,
                             C7_L,
                             C7_M,
                             C7_N,
                             C7_O,
                             C7_P,
                             C7_Q,
                             C7_R,
                             C7_S,
                             C7_T,
                             C7_U,
                             C7_V,
                             C7_W,
                             C7_X,
                             C7_Y,
                             C7_Z,   // 122 0x7A, Z replacing z
                             C7_CBL, // 123 0x7B
                             C7_OR,
                             C7_CBR,
                             C7_TLD,
                             C7_DEL  // 127
                        };

//------------------------------------------------------------------------------
// @brief PRIVATE FUNCTIONS
//------------------------------------------------------------------------------
/**
 * @brief Init the Serial interface and the controller
 * @param  none
 * @return none
 */
static void _init(tm1637* const dev);


/**
 * @brief Generate Start condition for TM1637
 * @param  none
 * @return none
 */
static void _start(tm1637* const dev);

/**
 * @brief Generate Stop condition for TM1637
 * @param  none
 * @return none
 */
static void _stop(tm1637* const dev);

/**
 * @brief Send byte to TM1637
 * @param  int data
 * @return none
 */
static void _write(tm1637* const dev, uint8_t data);

/**
 * @brief  Read byte from TM1637
 * @return read byte
 */
static uint8_t _read(tm1637* const dev);

/**
 * @brief Write command and parameter to TM1637
 * @param  int cmd Command byte
 * @param  int data Parameters for command
 * @return none
 */
static void _write_cmd(tm1637* const dev, uint8_t cmd, uint8_t data);

/**
 * @brief Write a single character
 */
static int _putc(tm1637* const dev, int value);

//------------------------------------------------------------------------------
// @brief FUNCTIONS DEFINITIONS
//------------------------------------------------------------------------------
void tm1637_init(tm1637* const dev)
{
    if (!dev)
    {
        return;
    }
    dev->column  = 0;
    dev->columns_num = DISPLAY_NR_DIGITS;

    _init(dev);
}

//------------------------------------------------------------------------------
void tm1637_clear(tm1637* const dev)
{
    _start(dev);

    _write(dev, TM1637_ADDR_SET_CMD | 0x00); // Address set cmd, 0
    for (int cnt=0; cnt<TM1637_DISPLAY_MEM; cnt++)
    {
        _write(dev, 0x00); // data
    }
    _stop(dev);
}


//------------------------------------------------------------------------------
// void tm1637_write_data(char data, int address)
// {
//     _start();

//     _write(TM1637_ADDR_SET_CMD | (address & TM1637_ADDR_MSK)); // Set Address cmd
//     _write(data); // data

//     _stop();
// }


//------------------------------------------------------------------------------
void tm1637_write_data(tm1637* const dev, tm1637_display_data data, int length, int address)
{
    _start(dev);

    // Sanity check
    address &= TM1637_ADDR_MSK;
    if (length < 0)
    {
        length = 0;
    }

    if ((length + address) > TM1637_DISPLAY_MEM)
    {
        length = (TM1637_DISPLAY_MEM - address);
    }

    _write(dev, TM1637_ADDR_SET_CMD | address); // Set Address

    for (int idx=0; idx<length; idx++)
    {
        _write(dev, data[address + idx]); // data
    }

    _stop(dev);
}


//------------------------------------------------------------------------------
bool tm1637_get_keys(tm1637* const dev, tm1637_key_data *keydata)
{
    _start(dev);

    // Enable Key Read mode
    _write(dev, TM1637_DATA_SET_CMD | TM1637_KEY_RD | TM1637_ADDR_INC | TM1637_MODE_NORM); // Data set cmd, normal mode, auto incr, read data

    // Read keys
    // Bitpattern S0 S1 S2 K1 K2 1 1 1
    *keydata = _read(dev);

    _stop(dev);

    // Restore Data Write mode
    _write_cmd(dev, TM1637_DATA_SET_CMD, TM1637_DATA_WR | TM1637_ADDR_INC | TM1637_MODE_NORM); // Data set cmd, normal mode, auto incr, write data

    return (*keydata != TM1637_SW_NONE);
}


//------------------------------------------------------------------------------
void tm1637_set_brightness(tm1637* const dev, uint8_t brightness)
{
    dev->brightness = brightness & TM1637_BRT_MSK; // mask invalid bits

    _write_cmd(dev, TM1637_DSP_CTRL_CMD, dev->display_on_off | dev->brightness );  // Display control cmd, display on/off, brightness
}


//------------------------------------------------------------------------------
void tm1637_set_display(tm1637* const dev, bool on)
{
    if (on)
    {
        dev->display_on_off = TM1637_DSP_ON;
    }
    else
    {
        dev->display_on_off = TM1637_DSP_OFF;
    }

    _write_cmd(dev, TM1637_DSP_CTRL_CMD, dev->display_on_off | dev->brightness );  // Display control cmd, display on/off, brightness
}

//------------------------------------------------------------------------------
void tm1637_locate(tm1637* const dev, int column)
{
    // Sanity check
    if (column < 0)
    {
        column = 0;
    }
    if (column > (dev->columns_num - 1))
    {
        column = dev->columns_num - 1;
    }
    dev->column = column;
}

//------------------------------------------------------------------------------
void tm1637_set_icon(tm1637* const dev, tm1637_icon icon)
{
    int addr, icn;

    icn =  icon  & 0xFFFF;
    addr = (icon >> 24) & 0xFF;
    addr = (addr - 1);

    //Save char...and set bits for icon to write
    dev->display_buffer[addr] = dev->display_buffer[addr] | LO(icn);
    tm1637_write_data(dev, dev->display_buffer, TM1637_BYTES_PER_GRID, addr);
}


//------------------------------------------------------------------------------
void tm1637_clear_icon(tm1637* const dev, tm1637_icon icon)
{
    int addr, icn;

    icn =  icon  & 0xFFFF;
    addr = (icon >> 24) & 0xFF;
    addr = (addr - 1);

    //Save char...and clr bits for icon to write
    dev->display_buffer[addr] = dev->display_buffer[addr] & ~LO(icn);
    tm1637_write_data(dev, dev->display_buffer, TM1637_BYTES_PER_GRID, addr);
}


//------------------------------------------------------------------------------
void tm1637_set_udc(tm1637* const dev, uint8_t udc_idx, int udc_data)
{
    //Sanity check
    if (udc_idx > (DISPLAY_NR_UDC-1))
    {
        return;
    }
    // Mask out Icon bits?
    dev->ud_chars[udc_idx] = LO(udc_data);
}


//------------------------------------------------------------------------------
int columns(const tm1637* const dev)
{
    return dev->columns_num;
}



//------------------------------------------------------------------------------
// @brief PRIVATE FUNCTIONS DEFINITIONS
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void _init(tm1637* const dev)
{
    // Init Serial bus
    tm1637_set_dio_mode(dev, TM1637_DIO_OUTPUT);
    tm1637_delay_us(dev, 1);

    tm1637_set_dio(dev, TM1637_PIN_HIGH);
    tm1637_set_clk(dev, TM1637_PIN_HIGH);

    //init controller
    dev->display_on_off = TM1637_DSP_ON;
    dev->brightness = TM1637_BRT_DEF;
    _write_cmd(dev, TM1637_DSP_CTRL_CMD, dev->display_on_off | dev->brightness);                                 // Display control cmd, display on/off, brightness

    _write_cmd(dev, TM1637_DATA_SET_CMD, TM1637_DATA_WR | TM1637_ADDR_INC | TM1637_MODE_NORM); // Data set cmd, normal mode, auto incr, write data
}

//------------------------------------------------------------------------------
void _start(tm1637* const dev)
{
    tm1637_set_dio(dev, TM1637_PIN_LOW);
    tm1637_delay_us(dev, 1);
    tm1637_set_clk(dev, TM1637_PIN_LOW );
    tm1637_delay_us(dev, 1);
}


//------------------------------------------------------------------------------
void _stop(tm1637* const dev)
{
    tm1637_set_dio(dev, TM1637_PIN_LOW);
    tm1637_delay_us(dev, 1);
    tm1637_set_clk(dev, TM1637_PIN_HIGH);
    tm1637_delay_us(dev, 1);
    tm1637_set_dio(dev, TM1637_PIN_HIGH);
    tm1637_delay_us(dev,1 );
}


//------------------------------------------------------------------------------
void _write(tm1637* const dev, uint8_t data)
{
    for (int bit=0; bit<8; bit++)
    {
        //The TM1637 expects LSB first
        if (((data >> bit) & 0x01) == 0x01)
        {
            tm1637_set_dio(dev, TM1637_PIN_HIGH);
        }
        else
        {
            tm1637_set_dio(dev, TM1637_PIN_LOW);
        }
        tm1637_delay_us(dev, 1);
        tm1637_set_clk(dev, TM1637_PIN_HIGH);
        tm1637_delay_us(dev, 1);
        tm1637_set_clk(dev, TM1637_PIN_LOW );
        tm1637_delay_us(dev, 1);
    }

    tm1637_set_dio(dev, TM1637_PIN_HIGH);

    // Prepare DIO to read data
    tm1637_set_dio_mode(dev, TM1637_DIO_INPUT);
    tm1637_delay_us(dev, 3);

    // dummy Ack
    tm1637_set_clk(dev, TM1637_PIN_HIGH);
    tm1637_delay_us(dev, 1);
    //  _ack = _dio;
    tm1637_set_clk(dev, TM1637_PIN_LOW );
    tm1637_delay_us(dev, 1);

    // Return DIO to output mode
    tm1637_set_dio_mode(dev, TM1637_DIO_OUTPUT);
    tm1637_delay_us(dev, 3);

    tm1637_set_dio(dev, TM1637_PIN_HIGH); //idle
}

//------------------------------------------------------------------------------
uint8_t _read(tm1637* const dev)
{
    char keycode = 0;

    // Prepare DIO to read data
    tm1637_set_dio_mode(dev, TM1637_DIO_INPUT);
    tm1637_delay_us(dev, 3);

    for (int bit=0; bit<8; bit++)
    {
        // The TM1637 sends bitpattern: S0 S1 S2 K1 K2 1 1 1
        // Data is shifted out by the TM1637 on the falling edge of CLK
        // Observe sufficient delay to allow the Open Drain DIO to rise to H levels
        // Prepare to read next bit, LSB (ie S0) first.
        // The code below flips bits for easier matching with datasheet
        keycode = keycode << 1;

        tm1637_set_clk(dev, TM1637_PIN_HIGH);
        tm1637_delay_us(dev, 1);

        // Read next bit
        if (tm1637_get_dio(dev) == TM1637_PIN_HIGH)
        {
            keycode |= 0x01;
        }

        tm1637_set_clk(dev, TM1637_PIN_LOW );
        tm1637_delay_us(dev, 5); // Delay to allow for slow risetime
    }

    // Return DIO to output mode
    tm1637_set_dio_mode(dev, TM1637_DIO_OUTPUT);
    tm1637_delay_us(dev, 3);

    // dummy Ack
    tm1637_set_dio(dev, TM1637_PIN_LOW); //Ack
    tm1637_delay_us(dev, 1);

    tm1637_set_clk(dev, TM1637_PIN_HIGH);
    tm1637_delay_us(dev, 1);
    tm1637_set_clk(dev, TM1637_PIN_LOW);
    tm1637_delay_us(dev, 1);

    tm1637_set_dio(dev, TM1637_PIN_HIGH); //idle

    return keycode;
}


//------------------------------------------------------------------------------
void _write_cmd(tm1637* const dev, uint8_t cmd, uint8_t data)
{
    _start(dev);

    _write(dev, (cmd & TM1637_CMD_MSK) | (data & ~TM1637_CMD_MSK));

    _stop(dev);
}

//------------------------------------------------------------------------------
int _putc(tm1637* const dev, int value)
{
    //The DISPLAY mapping between Digit positions (Left to Right) and Grids is:
    //  GR1 GR2 GR3 GR4
    //The memory addresses or column numbers are:
    //   0   1   2   3

    int addr;
    bool validChar = false;
    char pattern   = 0x00;

    if ((value == '\n') || (value == '\r'))
    {
      //No character to write
      validChar = false;

      //Update Cursor
      dev->column = 0;
    }
    else if ((value == '.') || (value == ','))
    {
        //No character to write
        validChar = false;
        pattern = S7_DP; // placeholder for all DPs

        // Check to see that DP can be shown for current column
        if (dev->column > 0)
        {
            //Translate between dev->column and displaybuffer entries
            //Add DP to bitpattern of digit left of current column.
            addr = (dev->column - 1);
            //Save icons...and set bits for decimal point to write
            dev->display_buffer[addr] = dev->display_buffer[addr] | pattern;
            tm1637_write_data(dev, dev->display_buffer, TM1637_BYTES_PER_GRID, addr);
            //No Cursor Update
        }
    }
    else if ((value >= 0) && (value < DISPLAY_NR_UDC))
    {
        //Character to write
        validChar = true;
        pattern = dev->ud_chars[value];
    }

    //Display all ASCII characters
    else if ((value >= FONT_7S_START) && (value <= FONT_7S_END))
    {
        //Character to write
        validChar = true;
        pattern = FONT_7S[value - FONT_7S_START];
    }

    if (validChar)
    {
        //Character to write
        //Translate between dev->column and displaybuffer entries
        addr = dev->column;

        //Save icons...and set bits for character to write
        dev->display_buffer[addr] = (dev->display_buffer[addr] & MASK_ICON_GRID[dev->column]) | pattern;

        tm1637_write_data(dev, dev->display_buffer, TM1637_BYTES_PER_GRID, addr);

        //Update Cursor
        dev->column++;
        if (dev->column > (DISPLAY_NR_DIGITS - 1))
        {
            dev->column = 0;
        }
    } // if validChar

    return value;
}

//------------------------------------------------------------------------------

