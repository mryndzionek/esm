#ifndef APPS_BLINK_BOARD_BLUEPILL_INC_BOARD_H_
#define APPS_BLINK_BOARD_BLUEPILL_INC_BOARD_H_

#include "config.h"

#define BOARD_DMA_SPI_TX(_data, _size) \
    (void)_data;                       \
    do                                 \
    {                                  \
    } while (0)

#define board_usb_send(_data, _len)
#define board_read_matrix(_mat)

#endif /* APPS_BLINK_BOARD_BLUEPILL_INC_BOARD_H_ */
