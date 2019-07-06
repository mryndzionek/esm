#ifndef APPS_BLINK_BOARD_BLUEPILL_INC_BOARD_H_
#define APPS_BLINK_BOARD_BLUEPILL_INC_BOARD_H_

#include "stm32f1xx_hal.h"

typedef enum
{
    NEC_NOT_EXTENDED,
    NEC_EXTENDED
} NEC_TYPE;

typedef enum
{
    NEC_INIT,
    NEC_AGC_OK,
    NEC_AGC_FAIL,
} NEC_STATE;

typedef struct
{
    int rawTimerData[32];
    uint8_t decoded[4];

    NEC_STATE state;

    TIM_HandleTypeDef *timerHandle;

    uint32_t timerChannel;

    uint16_t timingBitBoundary;
    uint16_t timingAgcBoundary;
    NEC_TYPE type;
} NEC_t;

extern SPI_HandleTypeDef hspi1;
extern I2C_HandleTypeDef hi2c1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern NEC_t nec1;

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

void board_nec_start(NEC_t *handle);
void board_nec_stop(NEC_t *handle);

#endif /* APPS_BLINK_BOARD_BLUEPILL_INC_BOARD_H_ */
