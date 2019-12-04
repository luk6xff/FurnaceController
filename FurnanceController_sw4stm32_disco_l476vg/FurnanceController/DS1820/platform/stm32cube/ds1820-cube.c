
/**
 *  @brief:  Implementation of a DS1820 platform dependent [STM32CUBE] radio functions
 *  @author: luk6xff
 *  @email:  lukasz.uszko@gmail.com
 *  @date:   2019-12-01
 */

#include "ds1820-cube.h"

// GPIO
static GPIO_TypeDef* _parasite_port;
static GPIO_TypeDef* _data_port;
static uint16_t _parasite_pin;
static uint16_t _data_pin;
// UART
UART_HandleTypeDef* _uart;

//------------------------------------------------------------------------------
void ds1820_cube_init(UART_HandleTypeDef* uart,
					  GPIO_TypeDef* GPIO_DATA_Port, uint16_t GPIO_DATA_Pin,
					  GPIO_TypeDef* GPIO_PARASITE_Port, uint16_t GPIO_PARASITE_Pin);
{
	_uart = uart;
	_parasite_port = GPIO_PARASITE_port;
	_parasite_pin = GPIO_PARASITE_pin;
	_data_port = GPIO_DATA_Port;
	_data_pin = GPIO_DATA_Pin;

    if (_parasite_port == NULL)
    {
        ds1820_init(false);
        return;
    }
    ds1820_init(true);
    ds1820_init();
}

//------------------------------------------------------------------------------
void ds1820_cube_deinit()
{
	// Empty
}

//------------------------------------------------------------------------------
void ds1820_set_parasite_pin(bool enable)
{
    if (enable)
    {
    	HAL_GPIO_WritePin(_parasite_port, _parasite_pin, GPIO_PIN_SET);
        return;
    }
	HAL_GPIO_WritePin(_parasite_port, _parasite_pin, GPIO_PIN_RESET);
}


//-----------------------------------------------------------------------------
