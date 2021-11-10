#ifndef __HW_CONFIG_H__
#define __HW_CONFIG_H__



#define BOARD_MBED_BLUEPILL_F103C8 true
#define BOARD_MBED_DISCO_L476VG false



#if BOARD_MBED_BLUEPILL_F103C8
// AT24CXX
#define AT24CXX_I2C_ADDR        0x07 // A0=1, A1=1, A2=1
#define AT24CXX_SDA             PB_7
#define AT24CXX_SCL             PB_6
#define AT24CXX_WP              PB_1
// DS3231
#define DS3231_I2C_ADDR         0x68
#define DS3231_SDA              PB_7
#define DS3231_SCL              PB_6
// TM1637
#define TM1637_DIO              PA_7
#define TM1637_CLK              PA_6
// DS18B20
#define DS18B20_SENSORS_NUM     1
#define DS18B20_DATA_PIN        PA_10
// BUTTONS
#define BTN_OK                  PB_4
#define BTN_DOWN                PB_3
#define BTN_UP                  PA_15
// RELAY
#define RELAY_PIN               PA_9
// DEBUG_UART_TX
#define DEBUG_UART              PA_4


#elif BOARD_MBED_DISCO_L476VG
// AT24CXX
#define AT24CXX_I2C_ADDR        0x07 // A0=1, A1=1, A2=1
#define AT24CXX_SDA             PB_7
#define AT24CXX_SCL             PB_6
#define AT24CXX_WP              PB_8
// DS3231
#define DS3231_I2C_ADDR         0x68
#define DS3231_SDA              PB_7
#define DS3231_SCL              PB_6
// TM1637
#define TM1637_DIO              PA_0
#define TM1637_CLK              PA_5
// DS18B20
#define DS18B20_SENSORS_NUM     1
#define DS18B20_DATA_PIN        PE_12
// BUTTONS
#define BTN_OK                  PE_13
#define BTN_DOWN                PE_14
#define BTN_UP                  PE_15
// RELAY
#define RELAY_PIN               PE_11


#endif



#endif //__HW_CONFIG_H__