#ifndef APPS_BLINK_BOARD_BLUEPILL_INC_BOARD_H_
#define APPS_BLINK_BOARD_BLUEPILL_INC_BOARD_H_

#include <stdbool.h>
#include "stm32f1xx_hal.h"

#include "usb_device.h"
#include "usbd_hid.h"

#include "config.h"

extern SPI_HandleTypeDef hspi2;
extern USBD_HandleTypeDef hUsbDeviceFS;

#define BOARD_DMA_SPI_TX(_data, _size)                                    \
    do                                                                    \
    {                                                                     \
        HAL_StatusTypeDef s = HAL_SPI_Transmit_DMA(&hspi2, _data, _size); \
        ESM_ASSERT(s == HAL_OK);                                          \
    } while (0)

#define BOARD_DEBOUNCER_STATE int

#define board_usb_send(_data, _len) USBD_HID_SendReport(&hUsbDeviceFS, _data, _len);
void board_read_matrix(bool (*const matrix)[N_COLS][N_ROWS]);

#endif /* APPS_BLINK_BOARD_BLUEPILL_INC_BOARD_H_ */
