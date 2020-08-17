#ifndef APPS_BLINK_BOARD_BLUEPILL_INC_BOARD_H_
#define APPS_BLINK_BOARD_BLUEPILL_INC_BOARD_H_

#include <stdbool.h>
#include "stm32f1xx_hal.h"

#include "usb_device.h"
#include "usbd_hid.h"

#include "config.h"

extern USBD_HandleTypeDef hUsbDeviceFS;
extern SPI_HandleTypeDef hspi2;
extern const uint32_t board_heat_colormap[256];

#define BOARD_DMA_SPI_TX(_data, _size)                                    \
    do                                                                    \
    {                                                                     \
        HAL_StatusTypeDef s = HAL_SPI_Transmit_DMA(&hspi2, _data, _size); \
        ESM_ASSERT(s == HAL_OK);                                          \
    } while (0)

#define BOARD_DEBOUNCER_STATE int

#define board_backlight_show() sk6812_show()
bool board_usb_send(uint8_t *data, uint16_t len);
void board_read_matrix(bool (*const matrix)[N_COLS][N_ROWS]);
void board_ledpos_to_xy(uint8_t p, uint8_t *xp, uint8_t *yp);

#endif /* APPS_BLINK_BOARD_BLUEPILL_INC_BOARD_H_ */
