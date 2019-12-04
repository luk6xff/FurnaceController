
/**
 *  @brief:  Implementation of a DS1820 platform dependent [STM32CUBE] radio functions
 *  @author: luk6xff
 *  @email:  lukasz.uszko@gmail.com
 *  @date:   2019-12-02
 */


#ifndef __DS1820_CUBE_H__
#define __DS1820_CUBE_H__

#include "stm32f0xx_hal.h"
#include "../../ds1820.h"


void ds1820_cube_init(UART_HandleTypeDef* uart,
					  GPIO_TypeDef* GPIO_DATA_Port, uint16_t GPIO_DATA_Pin,
					  GPIO_TypeDef* GPIO_PARASITE_Port, uint16_t GPIO_PARASITE_Pin);

void ds1820_cube_deinit();

#endif // __DS1820_CUBE_H__
