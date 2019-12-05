/* mbed TM1637 Library, for TM1637 LED controller
 * Copyright (c) 2016, v01: WH, Initial version
 *               2017, v02: WH, Added RobotDyn 6 Digit module,
 *                          Added Eyewink 6 Digit + 6 Keys module,
 *                          Constructor adapted to 2 pins: dio, clk
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, inclumosig without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUmosiG BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#include "mbed.h"
#include "TM1637.h"


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
//
//#define FONT_7S_START     0x20
//#define FONT_7S_END       0x7F
//#define FONT_7S_NR_CHARS (FONT_7_END - FONT_7S_START + 1)

//display all ASCII characters
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



/** Constructor for class for driving TM1637 LED controller with Serial bus interface device.
 *  @brief Supports 6 digits @ 8 segments. Also supports upto 16 Keys.
 *
 *  @param  PinName dio Serial bus DIO pin
 *  @param  PinName clk Serial bus CLK pin
*/
TM1637::TM1637(PinName dio, PinName clk) : _dio(dio), _clk(clk) {

  _column  = 0;
  _columns = DISPLAY_NR_DIGITS;

  _init();
}


/** Init the Serial interface and the controller
  * @param  none
  * @return none
  */
void TM1637::_init(){

//TM1637 uses a Serial bus that looks like I2C, but really is not.
//It has Start and Stop conditions like I2C and an Ack pulse, but instead of slaveaddresses and a RW bit it just transmits commands and data.

//init Serial bus
  _dio.output();
//  _dio.mode(PullUp);
  wait_us(1);

  _dio=1;
  _clk=1;

//init controller
  _display = TM1637_DSP_ON;
  _bright  = TM1637_BRT_DEF;
  _writeCmd(TM1637_DSP_CTRL_CMD, _display | _bright );                                 // Display control cmd, display on/off, brightness

  _writeCmd(TM1637_DATA_SET_CMD, TM1637_DATA_WR | TM1637_ADDR_INC | TM1637_MODE_NORM); // Data set cmd, normal mode, auto incr, write data
}


/** Clear the screen and locate to 0
 */
void TM1637::cls() {

  _start();

  _write(TM1637_ADDR_SET_CMD | 0x00); // Address set cmd, 0
  for (int cnt=0; cnt<TM1637_DISPLAY_MEM; cnt++) {
    _write(0x00); // data
  }

  _stop();
}

/** Set Brightness
  *
  * @param  char brightness (3 significant bits, valid range 0..7 (1/16 .. 14/14 dutycycle)
  * @return none
  */
void TM1637::setBrightness(char brightness){

  _bright = brightness & TM1637_BRT_MSK; // mask invalid bits

  _writeCmd(TM1637_DSP_CTRL_CMD, _display | _bright );  // Display control cmd, display on/off, brightness
}

/** Set the Display mode On/off
  *
  * @param bool display mode
  */
void TM1637::setDisplay(bool on) {

  if (on) {
    _display = TM1637_DSP_ON;
  }
  else {
    _display = TM1637_DSP_OFF;
  }

  _writeCmd(TM1637_DSP_CTRL_CMD, _display | _bright );  // Display control cmd, display on/off, brightness
}

/** Write databyte to TM1637
  *  @param  int address display memory location to write byte
  *  @param  char data byte written at given address
  *  @return none
  */
void TM1637::writeData(char data, int address) {

  _start();

  _write(TM1637_ADDR_SET_CMD | (address & TM1637_ADDR_MSK)); // Set Address cmd
  _write(data); // data

  _stop();
}

/** Write Display datablock to TM1637
  *  @param  DisplayData_t data Array of TM1637_DISPLAY_MEM (=16) bytes for displaydata
  *  @param  length number bytes to write (valid range 0..(TM1637_MAX_NR_GRIDS * TM1637_BYTES_PER_GRID) (=16), when starting at address 0)
  *  @param  int address display memory location to write bytes (default = 0)
  *  @return none
  */
void TM1637::writeData(DisplayData_t data, int length, int address) {

  _start();

// sanity check
  address &= TM1637_ADDR_MSK;
  if (length < 0) {length = 0;}
  if ((length + address) > TM1637_DISPLAY_MEM) {length = (TM1637_DISPLAY_MEM - address);}

//  _write(TM1637_ADDR_SET_CMD | 0x00); // Set Address at 0
  _write(TM1637_ADDR_SET_CMD | address); // Set Address

  for (int idx=0; idx<length; idx++) {
//    _write(data[idx]); // data
    _write(data[address + idx]); // data
  }

  _stop();
}

/** Read keydata block from TM1637
  *  @param  *keydata Ptr to bytes for keydata
  *  @return bool keypress True when at least one key was pressed
  */
bool TM1637::getKeys(KeyData_t *keydata) {

  _start();

  // Enable Key Read mode
  _write(TM1637_DATA_SET_CMD | TM1637_KEY_RD | TM1637_ADDR_INC | TM1637_MODE_NORM); // Data set cmd, normal mode, auto incr, read data

  // Read keys
  // Bitpattern S0 S1 S2 K1 K2 1 1 1
  *keydata = _read();
//  printf("Key = 0x%02x\r\n", *keydata);

  _stop();

  // Restore Data Write mode
  _writeCmd(TM1637_DATA_SET_CMD, TM1637_DATA_WR | TM1637_ADDR_INC | TM1637_MODE_NORM); // Data set cmd, normal mode, auto incr, write data

  return (*keydata != TM1637_SW_NONE);
}


/** Generate Start condition for TM1637
  *  @param  none
  *  @return none
  */
void TM1637::_start() {

  _dio=0;
  wait_us(1);
  _clk=0;
  wait_us(1);
}

/** Generate Stop condition for TM1637
  *  @param  none
  *  @return none
  */
void TM1637::_stop() {

  _dio=0;
  wait_us(1);
  _clk=1;
  wait_us(1);
  _dio=1;
  wait_us(1);
}

/** Send byte to TM1637
  *  @param  int data
  *  @return none
  */
void TM1637::_write(int data) {

  for (int bit=0; bit<8; bit++) {
    //The TM1637 expects LSB first
    if (((data >> bit) & 0x01) == 0x01) {
      _dio=1;
    }
    else {
      _dio=0;
    }
    wait_us(1);
    _clk=1;
    wait_us(1);
    _clk=0;
    wait_us(1);
  }

  _dio=1;

  // Prepare DIO to read data
  _dio.input();
  wait_us(3);

  // dummy Ack
  _clk=1;
  wait_us(1);
//  _ack = _dio;
  _clk=0;
  wait_us(1);

  // Return DIO to output mode
  _dio.output();
  wait_us(3);

  _dio=1; //idle
}

/** Read byte from TM1637
  *  @return read byte
  */
char TM1637::_read() {
  char keycode = 0;

  // Prepare DIO to read data
  _dio.input();
  wait_us(3);

  for (int bit=0; bit<8; bit++) {

    //The TM1637 sends bitpattern: S0 S1 S2 K1 K2 1 1 1
    //Data is shifted out by the TM1637 on the falling edge of CLK
    //Observe sufficient delay to allow the Open Drain DIO to rise to H levels
    // Prepare to read next bit, LSB (ie S0) first.
    // The code below flips bits for easier matching with datasheet
    keycode = keycode << 1;

    _clk=1;
    wait_us(1);

    // Read next bit
    if (_dio) { keycode |= 0x01; }

    _clk=0;
    wait_us(5); // Delay to allow for slow risetime
  }

  // Return DIO to output mode
  _dio.output();
  wait_us(3);

  // dummy Ack
  _dio=0; //Ack
  wait_us(1);

  _clk=1;
  wait_us(1);
  _clk=0;
  wait_us(1);

  _dio=1; //idle

  return keycode;
}

/** Write command and parameter to TM1637
  *  @param  int cmd Command byte
  *  &Param  int data Parameters for command
  *  @return none
  */
void TM1637::_writeCmd(int cmd, int data){

  _start();

  _write((cmd & TM1637_CMD_MSK) | (data & ~TM1637_CMD_MSK));

  _stop();
}


/** Locate cursor to a screen column
  *
  * @param column  The horizontal position from the left, indexed from 0
  */
void TM1637::locate(int column) {
  //sanity check
  if (column < 0) {column = 0;}
  if (column > (_columns - 1)) {column = _columns - 1;}

  _column = column;
}


/** Number of screen columns
  *
  * @param none
  * @return columns
  */
int TM1637::columns() {
    return _columns;
}


/** Clear the screen and locate to 0
  * @param bool clrAll Clear Icons also (default = false)
  */
// void TM1637::cls(bool clrAll) {

//   if (clrAll) {
//     //clear local buffer (inclumosig Icons)
//     for (int idx=0; idx < DISPLAY_NR_GRIDS; idx++) {
//       _displaybuffer[idx] = 0x00;
//     }
//   }
//   else {
//     //clear local buffer (preserving Icons)
//     for (int idx=0; idx < DISPLAY_NR_GRIDS; idx++) {
//       _displaybuffer[idx] = _displaybuffer[idx] & MASK_ICON_GRID[idx];
//     }
//   }

//   writeData(_displaybuffer, (DISPLAY_NR_GRIDS * TM1637_BYTES_PER_GRID), 0);

//   _column = 0;
// }

/** Set Icon
  *
  * @param Icon icon Enums Icon has Grid position encoded in 8 MSBs, Icon pattern encoded in 16 LSBs
  * @return none
  */
void TM1637::setIcon(Icon icon) {
  int addr, icn;

   icn =        icon  & 0xFFFF;
  addr = (icon >> 24) & 0xFF;
  addr = (addr - 1);

  //Save char...and set bits for icon to write
  _displaybuffer[addr] = _displaybuffer[addr] | LO(icn);
//  writeData(_displaybuffer, (DISPLAY_NR_GRIDS * TM1637_BYTES_PER_GRID), 0);
  writeData(_displaybuffer, TM1637_BYTES_PER_GRID, addr);
}

/** Clr Icon
  *
  * @param Icon icon Enums Icon has Grid position encoded in 8 MSBs, Icon pattern encoded in 16 LSBs
  * @return none
  */
void TM1637::clrIcon(Icon icon) {
  int addr, icn;

   icn =        icon  & 0xFFFF;
  addr = (icon >> 24) & 0xFF;
  addr = (addr - 1);

  //Save char...and clr bits for icon to write
  _displaybuffer[addr] = _displaybuffer[addr] & ~LO(icn);
//  writeData(_displaybuffer, (DISPLAY_NR_GRIDS * TM1637_BYTES_PER_GRID), 0);
  writeData(_displaybuffer, TM1637_BYTES_PER_GRID, addr);
}


/** Set User Defined Characters (UDC)
  *
  * @param unsigned char udc_idx  The Index of the UDC (0..7)
  * @param int udc_data           The bitpattern for the UDC (8 bits)
  */
void TM1637::setUDC(unsigned char udc_idx, int udc_data) {

  //Sanity check
  if (udc_idx > (DISPLAY_NR_UDC-1)) {
    return;
  }
  // Mask out Icon bits?

  _UDC_7S[udc_idx] = LO(udc_data);
}


/** Write a single character (Stream implementation)
  */
int TM1637::_putc(int value) {
//The CATALAX mapping between Digit positions (Left to Right) and Grids is:
//  GR1 GR2 GR3 GR4
//The memory addresses or column numbers are:
//   0   1   2   3

    int addr;
    bool validChar = false;
    char pattern   = 0x00;

    if ((value == '\n') || (value == '\r')) {
      //No character to write
      validChar = false;

      //Update Cursor
      _column = 0;
    }
    else if ((value == '.') || (value == ',')) {
      //No character to write
      validChar = false;
      pattern = S7_DP; // placeholder for all DPs

      // Check to see that DP can be shown for current column
      if (_column > 0) {
        //Translate between _column and displaybuffer entries
        //Add DP to bitpattern of digit left of current column.
        addr = (_column - 1);

        //Save icons...and set bits for decimal point to write
        _displaybuffer[addr] = _displaybuffer[addr] | pattern;
//        writeData(_displaybuffer, (DISPLAY_NR_GRIDS * TM1637_BYTES_PER_GRID));
        writeData(_displaybuffer, TM1637_BYTES_PER_GRID, addr);

        //No Cursor Update
      }
    }
    else if ((value >= 0) && (value < DISPLAY_NR_UDC)) {
      //Character to write
      validChar = true;
      pattern = _UDC_7S[value];
    }

    //display all ASCII characters
    else if ((value >= FONT_7S_START) && (value <= FONT_7S_END)) {
      //Character to write
      validChar = true;
      pattern = FONT_7S[value - FONT_7S_START];
    }

    if (validChar) {
      //Character to write

      //Translate between _column and displaybuffer entries
      addr = _column;

      //Save icons...and set bits for character to write
      _displaybuffer[addr] = (_displaybuffer[addr] & MASK_ICON_GRID[_column]) | pattern;

//      writeData(_displaybuffer, (DISPLAY_NR_GRIDS * TM1637_BYTES_PER_GRID));
      writeData(_displaybuffer, TM1637_BYTES_PER_GRID, addr);

      //Update Cursor
      _column++;
      if (_column > (DISPLAY_NR_DIGITS - 1)) {
        _column = 0;
      }

    } // if validChar

    return value;
}


// get a single character (Stream implementation)
int TM1637::_getc() {
    return -1;
}
