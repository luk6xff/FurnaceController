

#ifndef TM1637_H
#define TM1637_H


/**
 * @brief An interface for driving TM1637 LED controller
*/


// Segment bit positions for 7 Segment display using the DISPLAY and ROBOTDYN mapping for TM1637
// Modify this table for different 'bit-to-segment' mappings. The ASCII character defines and the FONT_7S const table below
// will be adapted automatically according to the bit-to-segment mapping. Obviously this will only work when the segment
// mapping is identical for every digit position. This will be the case unless the hardware designer really hates software developers.
//
//            A
//          -----
//         |     |
//       F |     | B
//         |  G  |
//          -----
//         |     |
//       E |     | C
//         |     |
//          -----   * DP
//            D
//
#define S7_A    0x0001
#define S7_B    0x0002
#define S7_C    0x0004
#define S7_D    0x0008
#define S7_E    0x0010
#define S7_F    0x0020
#define S7_G    0x0040
#define S7_DP   0x0080

//Mask for blending out and setting 7 segments digits
#define MASK_7S_ALL = (S7_A | S7_B | S7_C | S7_D | S7_E | S7_F | S7_G}

//The dispaly mapping between Digit positions or Columns (Left to Right) and Grids (ie memory address) are:
//The column numbers are:
//   0   1   2   3
//The Grids are:
//  GR1 GR2 GR3 GR4
//The memory addresses are:
//   0   1   2   3



//Icons Grid 1
#define S7_DP1  0x0080
#define S7_ICON_GR1 (0x0000)
//#define S7_ICON_GR1 (S7_DP1)

//Icons Grid 2
#define S7_DP2  0x0080
#define S7_COL2 0x0080
#define S7_ICON_GR2 (0x0000)
//#define S7_ICON_GR2 (S7_DP2)

//Icons Grid 3
#define S7_DP3  0x0080
#define S7_ICON_GR3 (0x0000)
//#define S7_ICON_GR3 (S7_DP3)

//Icons Grid 4
#define S7_DP4  0x0080
#define S7_ICON_GR4 (0x0000)
//#define S7_ICON_GR4 (S7_DP4)

//Icons Grid 5
#define S7_DP5  0x0080
#define S7_ICON_GR5 (0x0000)
//#define S7_ICON_GR5 (S7_DP5)

//Icons Grid 6
#define S7_DP6  0x0080
#define S7_ICON_GR6 (0x0000)
//#define S7_ICON_GR6 (S7_DP6)


//Mask for blending out and restoring Icons
extern const char MASK_ICON_GRID[];


// ASCII Font definitions for segments in each character
//
//32 0x20  Symbols
#define C7_SPC  (0x0000)
#define C7_EXC  (S7_B | S7_C) //!
#define C7_QTE  (S7_B | S7_F) //"
#define C7_HSH  (S7_C | S7_D | S7_E | S7_G) //#
#define C7_DLR  (S7_A | S7_C | S7_D | S7_F | S7_G) //$
#define C7_PCT  (S7_C | S7_F) //%
#define C7_AMP  (S7_A | S7_C | S7_D | S7_E | S7_F | S7_G) //&
#define C7_ACC  (S7_B) //'
#define C7_LBR  (S7_A | S7_D | S7_E | S7_F) //(
#define C7_RBR  (S7_A | S7_B | S7_C | S7_D) //)
#define C7_MLT  (S7_B | S7_C | S7_E | S7_F | S7_G)  //*
#define C7_PLS  (S7_B | S7_C | S7_G) //+
#define C7_CMA  (S7_DP)
#define C7_MIN  (S7_G)
#define C7_DPT  (S7_DP)
#define C7_RS   (S7_B | S7_E  | S7_G)  // /

//48 0x30  Digits
#define C7_0    (S7_A | S7_B | S7_C | S7_D | S7_E | S7_F)
#define C7_1    (S7_B | S7_C)
#define C7_2    (S7_A | S7_B | S7_D | S7_E | S7_G)
#define C7_3    (S7_A | S7_B | S7_C | S7_D | S7_G)
#define C7_4    (S7_B | S7_C | S7_F | S7_G)
#define C7_5    (S7_A | S7_C | S7_D | S7_F | S7_G)
#define C7_6    (S7_A | S7_C | S7_D | S7_E | S7_F | S7_G)
#define C7_7    (S7_A | S7_B | S7_C)
#define C7_8    (S7_A | S7_B | S7_C | S7_D | S7_E | S7_F | S7_G)
#define C7_9    (S7_A | S7_B | S7_C | S7_D | S7_F | S7_G)

//58 0x3A
#define C7_COL  (S7_D | S7_G) // :
#define C7_SCL  (S7_D | S7_G) // ;
#define C7_LT   (S7_D | S7_E | S7_G)             // <
#define C7_EQ   (S7_D | S7_G)                    // =
#define C7_GT   (S7_C | S7_D | S7_G)             // >
#define C7_QM   (S7_A | S7_B | S7_E | S7_G)      // ?
#define C7_AT   (S7_A | S7_B | S7_C | S7_D | S7_E  | S7_G)  // @

//65 0x41  Upper case alphabet
#define C7_A    (S7_A | S7_B | S7_C | S7_E | S7_F | S7_G )
#define C7_B    (S7_C | S7_D | S7_E | S7_F | S7_G)
#define C7_C    (S7_A | S7_D | S7_E | S7_F)
#define C7_D    (S7_B | S7_C | S7_D | S7_E | S7_G)
#define C7_E    (S7_A | S7_D | S7_E | S7_F | S7_G)
#define C7_F    (S7_A | S7_E | S7_F | S7_G)

#define C7_G    (S7_A | S7_C | S7_D | S7_E | S7_F)
#define C7_H    (S7_B | S7_C | S7_E | S7_F | S7_G)
#define C7_I    (S7_B | S7_C)
#define C7_J    (S7_B | S7_C | S7_D | S7_E)
#define C7_K    (S7_B | S7_C | S7_E | S7_F | S7_G)
#define C7_L    (S7_D | S7_E | S7_F)
#define C7_M    (S7_A | S7_C | S7_E)
#define C7_N    (S7_A | S7_B | S7_C | S7_E | S7_F)
#define C7_O    (S7_A | S7_B | S7_C | S7_D | S7_E | S7_F)
#define C7_P    (S7_A | S7_B | S7_E | S7_F | S7_G)
#define C7_Q    (S7_A | S7_B | S7_C | S7_F | S7_G)
#define C7_R    (S7_E | S7_G )
#define C7_S    (S7_A | S7_C | S7_D | S7_F | S7_G)
#define C7_T    (S7_D | S7_E | S7_F | S7_G)
#define C7_U    (S7_B | S7_C | S7_D | S7_E | S7_F)
#define C7_V    (S7_B | S7_C | S7_D | S7_E | S7_F)
#define C7_W    (S7_B | S7_D | S7_F)
#define C7_X    (S7_B | S7_C | S7_E | S7_F | S7_G)
#define C7_Y    (S7_B | S7_C | S7_D | S7_F | S7_G)
#define C7_Z    (S7_A | S7_B | S7_D | S7_E | S7_G)

//91 0x5B
#define C7_SBL  (S7_A | S7_D | S7_E | S7_F) // [
#define C7_LS   (S7_C | S7_F | S7_G)        // left slash
#define C7_SBR  (S7_A | S7_B | S7_C | S7_D) // ]
#define C7_PWR  (S7_A | S7_B | S7_F)        // ^
#define C7_UDS  (S7_D)                      // _
#define C7_DSH  (S7_F)                      // `

//97 0x61  Lower case alphabet
#define C7_a     C7_A
#define C7_b     C7_B
#define C7_c     C7_C
#define C7_d     C7_D
#define C7_e     C7_E
#define C7_f     C7_H

#define C7_g     C7_G
#define C7_h     C7_H
#define C7_i     C7_I
#define C7_j     C7_J
#define C7_k     C7_K
#define C7_l     C7_L
#define C7_m     C7_M
//#define C7_n     C7_N
#define C7_n    (S7_C | S7_E | S7_G)
//#define C7_o     C7_O
#define C7_o    (S7_C | S7_D | S7_E | S7_G)
#define C7_p     C7_P
#define C7_q     C7_Q
//#define C7_r     C7_R
#define C7_r    (S7_E | S7_G)
#define C7_s     C7_S
#define C7_t     C7_T
#define C7_u     C7_U
#define C7_v     C7_V
#define C7_w     C7_W
#define C7_x     C7_X
#define C7_y     C7_Y
#define C7_z     C7_Z

//123 0x7B
#define C7_CBL  (S7_A | S7_D | S7_E | S7_F)        // {
#define C7_OR   (S7_B | S7_C)                      // |
#define C7_CBR  (S7_A | S7_B | S7_C | S7_D)        // }
#define C7_TLD  (S7_B | S7_E | S7_G )              // ~
#define C7_DEL  (0x0000)


//User Defined Characters (some examples)
#define C7_DGR   (S7_A | S7_B | S7_F | S7_G)  //Degrees

// Font data selection for transmission to TM1637 memory
#define LO(x)  ( x & 0xFF)
#define HI(x)  ((x >> 8) & 0xFF)


// ASCII Font definition table
//
#define FONT_7S_START     0x20
#define FONT_7S_END       0x7F
//#define FONT_7S_NR_CHARS (FONT_7S_END - FONT_7S_START + 1)
extern const short FONT_7S[];


//TM1637 Display data
#define TM1637_MAX_NR_GRIDS    6
#define TM1637_BYTES_PER_GRID  1

//Significant bits Keymatrix data
//#define TM1638_KEY_MSK      0xFF

//Memory size in bytes for Display and Keymatrix
#define TM1637_DISPLAY_MEM  (TM1637_MAX_NR_GRIDS * TM1637_BYTES_PER_GRID)
#define TM1637_KEY_MEM         2

//Reserved bits for commands
#define TM1637_CMD_MSK      0xC0

//Data setting commands
#define TM1637_DATA_SET_CMD 0x40
#define TM1637_DATA_WR      0x00
#define TM1637_KEY_RD       0x02
#define TM1637_ADDR_INC     0x00
#define TM1637_ADDR_FIXED   0x04
#define TM1637_MODE_NORM    0x00
#define TM1637_MODE_TEST    0x08

//Address setting commands
#define TM1637_ADDR_SET_CMD 0xC0
#define TM1637_ADDR_MSK     0x07 //0..5

//Display control commands
#define TM1637_DSP_CTRL_CMD 0x80
#define TM1637_BRT_MSK      0x07
#define TM1637_BRT0         0x00 //Pulsewidth 1/16
#define TM1637_BRT1         0x01
#define TM1637_BRT2         0x02
#define TM1637_BRT3         0x03
#define TM1637_BRT4         0x04
#define TM1637_BRT5         0x05
#define TM1637_BRT6         0x06
#define TM1637_BRT7         0x07 //Pulsewidth 14/16

#define TM1637_BRT_DEF      TM1637_BRT3

#define TM1637_DSP_OFF      0x00
#define TM1637_DSP_ON       0x08


//Access to 16 Switches
//S0 S1 S2 K1 K2 1 1 1
//K1,K2 = 0 1
#define TM1637_SW1_BIT      0xEF
#define TM1637_SW2_BIT      0x6F
#define TM1637_SW3_BIT      0xAF
#define TM1637_SW4_BIT      0x2F
#define TM1637_SW5_BIT      0xCF
#define TM1637_SW6_BIT      0x4F
#define TM1637_SW7_BIT      0x8F
#define TM1637_SW8_BIT      0x0F

//K1,K2 = 1 0
#define TM1637_SW9_BIT      0xF7
#define TM1637_SW10_BIT     0x77
#define TM1637_SW11_BIT     0xB7
#define TM1637_SW12_BIT     0x37
#define TM1637_SW13_BIT     0xD7
#define TM1637_SW14_BIT     0x57
#define TM1637_SW15_BIT     0x97
#define TM1637_SW16_BIT     0x17

#define TM1637_SW_NONE      0xFF


// Modify for your screen
#define DISPLAY_NR_GRIDS  4
#define DISPLAY_NR_DIGITS 4
#define DISPLAY_NR_UDC    8


/** A class for driving TM1637 LED controller
 *
 * @brief Supports 4 Grids @ 4 Segments and 16 Keys.
 *        Serial bus interface device.
 */
class TM1637 : public Stream
{
 public:

  /** Datatype for displaydata */
  typedef char DisplayData_t[TM1637_DISPLAY_MEM];

  /** Datatypes for keymatrix data */
  typedef char KeyData_t;


 /** Constructor for class for driving TM1637 LED controller
  *
  * @brief Supports 4 Digits of 7 Segments + DP (or Colon for Digit2 on some models).
  *        Also Supports up to 16 Keys. Serial bus interface device.
  *
  *  @param  PinName dio Serial bus DIO pin
  *  @param  PinName sck Serial bus CLK pin
  */
  TM1637(PinName dio, PinName clk);

  /** Clear the screen and locate to 0
   */
  void cls();



   /** Write databyte to TM1637
     *  @param  char data byte written at given address
     *  @param  int address display memory location to write byte
     *  @return none
     */
    void writeData(char data, int address);

   /** Write Display datablock to TM1637
    *  @param  DisplayData_t data Array of TM1637_DISPLAY_MEM (=4) bytes for displaydata
    *  @param  length number bytes to write (valid range 0..(DISPLAY_NR_GRIDS * TM1637_BYTES_PER_GRID) (=4), when starting at address 0)
    *  @param  int address display memory location to write bytes (default = 0)
    *  @return none
    */
    void writeData(DisplayData_t data, int length = (DISPLAY_NR_GRIDS * TM1637_BYTES_PER_GRID), int address = 0);

  /** Read keydata block from TM1637
   *  @param  *keydata Ptr to bytes for keydata
   *  @return bool keypress True when at least one key was pressed
   *
   */
  bool getKeys(KeyData_t *keydata);

  /** Set Brightness
    *
    * @param  char brightness (3 significant bits, valid range 0..7 (1/16 .. 14/16 dutycycle)
    * @return none
    */
  void setBrightness(char brightness = TM1637_BRT_DEF);

  /** Set the Display mode On/off
    *
    * @param bool display mode
    */
  void setDisplay(bool on);





  /** Enums for Icons */
  //  Grid encoded in 8 MSBs, Icon pattern encoded in 16 LSBs
  enum Icon {
    DP1   = ( 1<<24) | S7_DP1,  /**<  Digit 1 */
    DP2   = ( 2<<24) | S7_DP2,  /**<  Digit 2 */
    DP3   = ( 3<<24) | S7_DP3,  /**<  Digit 3 */
    DP4   = ( 4<<24) | S7_DP4,  /**<  Digit 4 */

    COL2  = ( 2<<24) | S7_DP2,  /**<  Column 2 */
  };

  typedef char UDCData_t[DISPLAY_NR_UDC];



     /** Locate cursor to a screen column
     *
     * @param column  The horizontal position from the left, indexed from 0
     */
    void locate(int column);


    /** Set Icon
     *
     * @param Icon icon Enums Icon has Grid position encoded in 8 MSBs, Icon pattern encoded in 16 LSBs
     * @return none
     */
    void setIcon(Icon icon);

    /** Clr Icon
     *
     * @param Icon icon Enums Icon has Grid position encoded in 8 MSBs, Icon pattern encoded in 16 LSBs
     * @return none
     */
    void clrIcon(Icon icon);

   /** Set User Defined Characters (UDC)
     *
     * @param unsigned char udc_idx   The Index of the UDC (0..7)
     * @param int udc_data            The bitpattern for the UDC (16 bits)
     */
    void setUDC(unsigned char udc_idx, int udc_data);


   /** Number of screen columns
    *
    * @param none
    * @return columns
    */
    int columns();


protected:
    // Stream implementation functions
    virtual int _putc(int value);
    virtual int _getc();

 private:

    DigitalInOut _dio;
    DigitalOut   _clk;

    int _column;
    int _columns;

    DisplayData_t _displaybuffer;
    UDCData_t _UDC_7S;

    char _display;
    char _bright;

  /** Init the Serial interface and the controller
    * @param  none
    * @return none
    */
    void _init();


  /** Generate Start condition for TM1637
    *  @param  none
    *  @return none
    */
    void _start();

  /** Generate Stop condition for TM1637
    *  @param  none
    *  @return none
    */
    void _stop();

  /** Send byte to TM1637
    *  @param  int data
    *  @return none
    */
    void _write(int data);

  /** Read byte from TM1637
    *  @return read byte
    */
    char _read();

  /** Write command and parameter to TM1637
    *  @param  int cmd Command byte
    *  &Param  int data Parameters for command
    *  @return none
    */
    void _writeCmd(int cmd, int data);


};






#endif