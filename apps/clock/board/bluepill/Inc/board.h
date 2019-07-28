#ifndef APPS_BLINK_BOARD_BLUEPILL_INC_BOARD_H_
#define APPS_BLINK_BOARD_BLUEPILL_INC_BOARD_H_

#include "stm32f1xx_hal.h"
#include "nec.h"

struct _NEC_HW_CTX
{
    TIM_HandleTypeDef *timerHandle;
    uint32_t timerChannel;
};

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

#define BOARD_START_SOUND(_freq, _dur)                          \
    do                                                          \
    {                                                           \
        (void)_dur;                                             \
        htim3.Init.Prescaler = SystemCoreClock / (255 * n) - 1; \
        if (HAL_TIM_Base_Init(&htim3) != HAL_OK)                \
        {                                                       \
            Error_Handler();                                    \
        }                                                       \
        HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);               \
    } while (0)

#define BOARD_STOP_SOUND()                       \
    do                                           \
    {                                            \
        HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_3); \
    } while (0)

void board_nec_start(NEC_t *handle);
void board_nec_stop(NEC_t *handle);

#endif /* APPS_BLINK_BOARD_BLUEPILL_INC_BOARD_H_ */
