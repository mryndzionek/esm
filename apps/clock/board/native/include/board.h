#ifndef APPS_BLINK_BOARD_BLUEPILL_INC_BOARD_H_
#define APPS_BLINK_BOARD_BLUEPILL_INC_BOARD_H_

#include <stdint.h>
#include "nec.h"

struct _NEC_HW_CTX
{

};

extern NEC_t nec1;

#define BOARD_DMA_SPI_TX(_data, _size) \
    (void)_data;                       \
    do                                 \
    {                                  \
    } while (0)

#define BOARD_I2C_TX(_addr, _data, _size) \
    do                                    \
    {                                     \
    } while (0)

#define BOARD_I2C_RX(_addr, _data, _size) \
    do                                    \
    {                                     \
    } while (0)

#define BOARD_START_SOUND(_freq) \
    (void)_freq;                 \
    do                           \
    {                            \
    } while (0)

#define BOARD_STOP_SOUND() \
    do                     \
    {                      \
    } while (0)

void board_nec_start(NEC_t *handle);
void board_nec_stop(NEC_t *handle);

#endif /* APPS_BLINK_BOARD_BLUEPILL_INC_BOARD_H_ */
