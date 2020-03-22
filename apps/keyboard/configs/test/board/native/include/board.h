#ifndef APPS_BLINK_BOARD_BLUEPILL_INC_BOARD_H_
#define APPS_BLINK_BOARD_BLUEPILL_INC_BOARD_H_

#include "config.h"

#define BOARD_DMA_SPI_TX(_data, _size) \
    (void)_data;                       \
    do                                 \
    {                                  \
    } while (0)

#define BOARD_DEBOUNCER_STATE uint8_t

#define board_usb_send(_data, _len) (true)
#define board_read_matrix(_mat)
#define board_backlight_show() sk6812_show()

#endif /* APPS_BLINK_BOARD_BLUEPILL_INC_BOARD_H_ */
