#ifndef APPS_BLINK_BOARD_BLUEPILL_INC_BOARD_H_
#define APPS_BLINK_BOARD_BLUEPILL_INC_BOARD_H_

#include "stm32f1xx_hal.h"

extern SPI_HandleTypeDef hspi1;
extern I2C_HandleTypeDef hi2c1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;

#define BOARD_DMA_SPI_TX(_data, _size)                                    \
    do                                                                    \
    {                                                                     \
        HAL_StatusTypeDef s = HAL_SPI_Transmit_DMA(&hspi1, _data, _size); \
        ESM_ASSERT(s == HAL_OK);                                          \
    } while (0)

#define BOARD_I2C_TX(_addr, _data, _size)                                              \
    do                                                                                 \
    {                                                                                  \
        HAL_StatusTypeDef s = HAL_I2C_Master_Transmit(&hi2c1, _addr, _data, _size, 1); \
        ESM_ASSERT(s == HAL_OK);                                                       \
    } while (0)

#define BOARD_I2C_RX(_addr, _data, _size)                                             \
    do                                                                                \
    {                                                                                 \
        HAL_StatusTypeDef s = HAL_I2C_Master_Receive(&hi2c1, _addr, _data, _size, 1); \
        ESM_ASSERT(s == HAL_OK);                                                      \
    } while (0)

void board_init(void);

#endif /* APPS_BLINK_BOARD_BLUEPILL_INC_BOARD_H_ */
