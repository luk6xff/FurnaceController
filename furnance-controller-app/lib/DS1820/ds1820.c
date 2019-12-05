#include "ds1820.h"

//------------------------------------------------------------------------------
// @brief GLOBAL VARIABLES
//------------------------------------------------------------------------------
/** 
 * @brief ROM is a copy of the internal DS1820's ROM
 *        It is created during the ds1820_search_rom() or ds1820_search_alarm() commands
 *
 *        ds1820_rom[0] is the Dallas Family Code
 *        ds1820_rom[1] thru ds1820_rom[6] is the 48-bit unique serial number
 *        ds1820_rom[7] is the device CRC
*/
static char ds1820_rom[8];
#define FAMILY_CODE ds1820_rom[0]
#define FAMILY_CODE_DS1820 0x10
#define FAMILY_CODE_DS18S20 0x10
#define FAMILY_CODE_DS18B20 0x28

/** 
 * @brief RAM is a copy of the internal DS1820's RAM
 *        It's updated during the ds1820_read_ram() command
 *        which is automaticaly called from any function
 *        using the RAM values.
 */
static char ds1820_ram[9];

static bool _ds1820_done_flag;
static int  _ds1820_last_descrepancy;
static char _ds1820_search_ds1820_rom[8];
bool _ds1820_parasite_power;



//------------------------------------------------------------------------------
// @brief PRIVATE FUNCTIONS
//------------------------------------------------------------------------------
static char crc_byte (char crc, char byte);
static void match_rom(void);
static void skip_rom(void);
static bool ds1820_search_rom_routine(char command);
static void onewire_byte_out(char data);
static char onewire_byte_in(void);


//------------------------------------------------------------------------------
// @brief FUNCTIONS DEFINITIONS
//------------------------------------------------------------------------------
void ds1820_init(bool parasite_powered)
{
    _ds1820_parasite_power = parasite_powered;
    for(int i=0; i<8; i++)
    {
        ds1820_rom[i] = 0xFF;
    }
    for(int i=0; i<9; i++)
    {
        ds1820_ram[i] = 0x00;
    }
}

//------------------------------------------------------------------------------ 
void onewire_byte_out(char data)
{ // output data character (least sig bit first).
    int n;
    for (n=0; n<8; n++)
    {
        ds1820_onewire_bit_out(data & 0x01);
        data = data >> 1; // now the next bit is in the least sig bit position.
    }
}

//------------------------------------------------------------------------------ 
char onewire_byte_in()
{   // read byte, least sig byte first
    char answer = 0x00;
    int i;
    for (i=0; i<8; i++)
    {
        answer = answer >> 1; // shift over to make room for the next bit
        if (ds1820_onewire_bit_in())
            answer = answer | 0x80; // if the data port is high, make this bit a 1
    }
    return answer;
}

//------------------------------------------------------------------------------ 
bool ds1820_search_rom()
{
    return ds1820_search_rom_routine(0xF0);    // Search ROM command
}
 
//------------------------------------------------------------------------------
bool ds1820_search_alarm()
{
    return ds1820_search_rom_routine(0xEC);    // Search Alarm command
}

//------------------------------------------------------------------------------
bool ds1820_search_rom_routine(char command)
{
    int descrepancy_marker, ROM_bit_index;
    bool return_value, Bit_A, Bit_B;
    char byte_counter, bit_mask;
 
    return_value=false;
    if (!_ds1820_done_flag)
    {
        if (!ds1820_onewire_reset())
        {
            _ds1820_last_descrepancy = 0; // no devices present
        }
        else 
        {
            ROM_bit_index=1;
            descrepancy_marker=0;
            onewire_byte_out(command); // Search ROM command or Search Alarm command
            byte_counter = 0;
            bit_mask = 0x01;
            while (ROM_bit_index <= 64)
            {
                Bit_A = ds1820_onewire_bit_in();
                Bit_B = ds1820_onewire_bit_in();
                if (Bit_A & Bit_B)
                {
                    descrepancy_marker = 0; // data read error, this should never happen
                    ROM_bit_index = 0xFF;
                }
                else
                {
                    if (Bit_A | Bit_B)
                    {
                        // Set ROM bit to Bit_A
                        if (Bit_A)
                        {
                            _ds1820_search_ds1820_rom[byte_counter] = _ds1820_search_ds1820_rom[byte_counter] | bit_mask; // Set ROM bit to one
                        }
                        else
                        {
                            _ds1820_search_ds1820_rom[byte_counter] = _ds1820_search_ds1820_rom[byte_counter] & ~bit_mask; // Set ROM bit to zero
                        }
                    }
                    else
                    {
                        // both bits A and B are low, so there are two or more devices present
                        if (ROM_bit_index == _ds1820_last_descrepancy)
                        {
                            _ds1820_search_ds1820_rom[byte_counter] = _ds1820_search_ds1820_rom[byte_counter] | bit_mask; // Set ROM bit to one
                        }
                        else
                        {
                            if (ROM_bit_index > _ds1820_last_descrepancy)
                            {
                                _ds1820_search_ds1820_rom[byte_counter] = _ds1820_search_ds1820_rom[byte_counter] & ~bit_mask; // Set ROM bit to zero
                                descrepancy_marker = ROM_bit_index;
                            }
                            else
                            {
                                if ((_ds1820_search_ds1820_rom[byte_counter] & bit_mask) == 0x00)
                                {
                                    descrepancy_marker = ROM_bit_index;
                                }
                            }
                        }
                    }
                    ds1820_onewire_bit_out (_ds1820_search_ds1820_rom[byte_counter] & bit_mask);
                    ROM_bit_index++;
                    if (bit_mask & 0x80)
                    {
                        byte_counter++;
                        bit_mask = 0x01;
                    }
                    else
                    {
                        bit_mask = bit_mask << 1;
                    }
                }
            }
            _ds1820_last_descrepancy = descrepancy_marker;
            if (ROM_bit_index != 0xFF)
            {
                for(byte_counter=0;byte_counter<8;byte_counter++)
                {
                    ds1820_rom[byte_counter] = _ds1820_search_ds1820_rom[byte_counter];
                }
                return_value = true;
            }
        }
        if (_ds1820_last_descrepancy == 0)
        {
            _ds1820_done_flag = true;
        }
    }
    return return_value;
}

//------------------------------------------------------------------------------ 
void ds1820_search_rom_setup()
{
    _ds1820_done_flag = false;
    _ds1820_last_descrepancy = 0;
    int i;
    for (i=0; i<8; i++)
    {
        _ds1820_search_ds1820_rom[i]=0x00;
    }
}

//------------------------------------------------------------------------------ 
void ds1820_read_rom()
{
    // NOTE: This command can only be used when there is one DS1820 on the bus. If this command
    // is used when there is more than one slave present on the bus, a data collision will occur
    // when all the DS1820s attempt to respond at the same time.
    int i;
    ds1820_onewire_reset();
    onewire_byte_out(0x33);   // Read ROM id
    for (i=0; i<8; i++)
    {
        ds1820_rom[i] = onewire_byte_in();
    }
}

//------------------------------------------------------------------------------ 
void match_rom()
{
    // Used to select a specific device
    int i;
    ds1820_onewire_reset();
    onewire_byte_out( 0x55);  //Match ROM command
    for (i=0; i<8; i++)
    {
        onewire_byte_out(ds1820_rom[i]);
    }
}

//------------------------------------------------------------------------------ 
void skip_rom()
{
    ds1820_onewire_reset();
    onewire_byte_out(0xCC);   // Skip ROM command
}

//------------------------------------------------------------------------------ 
bool ds1820_rom_checksum_error()
{
    char crc=0x00;
    int i;
    for(i=0;i<7;i++) // Only going to shift the lower 7 bytes
        crc = crc_byte(crc, ds1820_rom[i]);
    // After 7 bytes crc should equal the 8th byte (ROM crc)
    return (crc!=ds1820_rom[7]); // will return true if there is a crc checksum error         
}

//------------------------------------------------------------------------------ 
bool ds1820_ram_checksum_error()
{
    char crc=0x00;
    int i;
    ds1820_read_ram();
    for(i=0;i<8;i++) // Only going to shift the lower 8 bytes
    {
        crc = crc_byte(crc, ds1820_ram[i]);
    }
    // After 8 bytes crc should equal the 9th byte (RAM crc)
    return (crc != ds1820_ram[8]); // will return true if there is a crc checksum error         
}

//------------------------------------------------------------------------------ 
char crc_byte(char crc, char byte)
{
    int j;
    for(j=0; j<8; j++)
    {
        if ((byte & 0x01 ) ^ (crc & 0x01))
        {
            // DATA ^ LSB crc = 1
            crc = crc >> 1;
            // Set the MSB to 1
            crc = crc | 0x80;
            // Check bit 3
            if (crc & 0x04) 
            {
                crc = crc & 0xFB; // Bit 3 is set, so clear it
            } else
            {
                crc = crc | 0x04; // Bit 3 is clear, so set it
            }
            // Check bit 4
            if (crc & 0x08)
            {
                crc = crc & 0xF7; // Bit 4 is set, so clear it
            } else
            {
                crc = crc | 0x08; // Bit 4 is clear, so set it
            }
        }
        else
        {
            // DATA ^ LSB crc = 0
            crc = crc>>1;
            // clear MSB
            crc = crc & 0x7F;
            // No need to check bits, with DATA ^ LSB crc = 0, they will remain unchanged
        }
        byte = byte >> 1;
    }
    return crc;
}

//------------------------------------------------------------------------------ 
void ds1820_convert_temperature(DS1820_Devices_t device)
{
    // Convert temperature into scratchpad RAM for all devices at once
    int delay_time = 750; // Default delay time
    char resolution;
    if (device == ALL)
    {
        skip_rom();          // Skip ROM command, will convert for ALL devices
    }
    else
    {
        match_rom();
        if (FAMILY_CODE == FAMILY_CODE_DS18B20)
        {
            resolution = ds1820_ram[4] & 0x60;
            if (resolution == 0x00) // 9 bits
            {
                delay_time = 94;
            }
            else if (resolution == 0x20) // 10 bits
            {
                delay_time = 188;
            }
            else if (resolution == 0x40) // 11 bits. Note 12bits uses the 750ms default
            {
                delay_time = 375;
            }
        }
    }
    onewire_byte_out(0x44);  // perform temperature conversion
    if (_ds1820_parasite_power)
    {
        ds1820_set_parasite_pin(true);       // Parasite power strong pullup
    }
    ds1820_delay_us(delay_time*1000);
    if (_ds1820_parasite_power)
    {
        ds1820_set_parasite_pin(false);
    }
}

//------------------------------------------------------------------------------ 
void ds1820_read_ram()
{
    // This will copy the DS1820's 9 bytes of RAM data
    // into the objects RAM array. Functions that use
    // RAM values will automaticly call this procedure.
    int i;
    match_rom();             // Select this device
    onewire_byte_out(0xBE);  //Read Scratchpad command
    for(i=0; i<9; i++)
    {
        ds1820_ram[i] = onewire_byte_in();
    }
}

//------------------------------------------------------------------------------ 
bool ds1820_set_configuration_bits(unsigned int resolution)
{
    bool answer = false;
    resolution = resolution - 9;
    if (resolution < 4)
    {
        resolution = resolution << 5; // align the bits
        ds1820_ram[4] = (ds1820_ram[4] & 0x60) | resolution; // mask out old data, insert new
        ds1820_write_scratchpad ((ds1820_ram[2]<<8) + ds1820_ram[3]);
        //ds1820_store_scratchpad (THIS);
        answer = true;
    }
    return answer;
}

//------------------------------------------------------------------------------ 
int ds1820_read_scratchpad()
{
    int answer;
    ds1820_read_ram();
    answer = (ds1820_ram[2]<<8) + ds1820_ram[3];
    return answer;
}
 
//------------------------------------------------------------------------------
void ds1820_write_scratchpad(int data)
{
    ds1820_ram[3] = data;
    ds1820_ram[2] = data>>8;
    match_rom();
    onewire_byte_out(0x4E);   // Copy scratchpad into DS1820 ram memory
    onewire_byte_out(ds1820_ram[2]); // T(H)
    onewire_byte_out(ds1820_ram[3]); // T(L)
    if (FAMILY_CODE == FAMILY_CODE_DS18B20)
    {
        onewire_byte_out(ds1820_ram[4]); // Configuration register
    }
}

//------------------------------------------------------------------------------ 
void ds1820_store_scratchpad(DS1820_Devices_t device)
{
    if (device == ALL)
    {
        skip_rom();          // Skip ROM command, will store for ALL devices
    }
    else
    {
        match_rom();
    }
    onewire_byte_out(0x48);   // Write scratchpad into E2 command
    if (_ds1820_parasite_power)
    {
        ds1820_set_parasite_pin(true);
    }
    ds1820_delay_us(10*1000);            // Parasite power strong pullup for 10ms
    if (_ds1820_parasite_power)
    {
        ds1820_set_parasite_pin(false);
    }
}

//------------------------------------------------------------------------------ 
int ds1820_recall_scratchpad(DS1820_Devices_t device)
{
    // This copies the E2 values into the DS1820's memory.
    // If you specify ALL this will return zero, otherwise
    // it will return the value of the scratchpad memory.
    int answer=0;
    if (device==ALL)
    {
        skip_rom();          // Skip ROM command, will recall for ALL devices
    }
    else
    {
        match_rom();
    }
    onewire_byte_out(0xB8);   // Recall E2 data to scratchpad command
    ds1820_delay_us(10*1000); // not sure I like polling for completion
                 // it could cause an infinite loop
    if (device==THIS)
    {
        ds1820_read_ram();
        answer = ds1820_read_scratchpad();
    }
    return answer;
}    

//------------------------------------------------------------------------------ 
float ds1820_read_temperature(DS1820_Scale_t scale)
{
    float answer, remaining_count, count_per_degree;
    int reading;
    ds1820_read_ram();
    reading = (ds1820_ram[1] << 8) + ds1820_ram[0];
    if (reading & 0x8000) // negative degrees C
    { 
        reading = 0-((reading ^ 0xffff) + 1); // 2's comp then convert to signed int
    }
    answer = reading +0.0; // convert to floating point
    if (FAMILY_CODE == FAMILY_CODE_DS18B20)
    {
        answer = answer / 8.0;
    }
    else
    {
        remaining_count = ds1820_ram[6];
        count_per_degree = ds1820_ram[7];
        answer = answer - 0.25 + (count_per_degree - remaining_count) / count_per_degree;
    }
    if (scale == CELSIUS)
    {
        answer = answer / 2.0;
    }
    else // FAHRENHEIT
    {
        answer = answer * 9.0 / 10.0 + 32.0;
    }
    return answer;
}

//------------------------------------------------------------------------------ 
bool ds1820_read_power_supply(DS1820_Devices_t device)
{
    // This will return true if the device (or all devices) are Vcc powered
    // This will return false if the device (or ANY device) is parasite powered
    if (device==ALL)
    {
        skip_rom();          // Skip ROM command, will poll for any device using parasite power
    }
    else
    {
        match_rom();
    }
    onewire_byte_out(0xB4);   // Read power supply command
    return ds1820_onewire_bit_in();
}

//------------------------------------------------------------------------------ 