#ifndef APPS_BLINK_BOARD_BLUEPILL_INC_BOARD_H_
#define APPS_BLINK_BOARD_BLUEPILL_INC_BOARD_H_

#include "stm32f1xx_hal.h"
#include "nec.h"

#define N_ROWS (1)
#define N_COLS (SK6812_LEDS_NUM)

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
extern IWDG_HandleTypeDef hiwdg;
extern const uint32_t board_heat_colormap[256];

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

#define BOARD_PLAY(_buf, _len)                                                \
    do                                                                        \
    {                                                                         \
        HAL_TIM_PWM_Start_DMA(&htim3, TIM_CHANNEL_3, (uint32_t *)_buf, _len); \
    } while (0)

#define BOARD_PAUSE()                                \
    do                                               \
    {                                                \
        HAL_TIM_PWM_Stop_DMA(&htim3, TIM_CHANNEL_3); \
    } while (0)

#define BOARD_DEBOUNCER_STATE int

#define board_backlight_show()       \
    {                                \
        esm_signal_t s = {           \
            .type = esm_sig_alarm,   \
            .sender = NULL,          \
            .receiver = strip1_esm}; \
        esm_send_signal(&s);         \
    }

#define BOARD_WDG_FEED() HAL_IWDG_Refresh(&hiwdg)

void board_nec_start(NEC_t *handle);
void board_nec_stop(NEC_t *handle);
void board_ledpos_to_xy(uint8_t p, uint8_t *xp, uint8_t *yp);

#endif /* APPS_BLINK_BOARD_BLUEPILL_INC_BOARD_H_ */
