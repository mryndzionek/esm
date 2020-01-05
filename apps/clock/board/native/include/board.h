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

#define BOARD_I2C_TX(_addr, _data, _size)  \
    do                                     \
    {                                      \
        board_i2c_tx(_addr, _data, _size); \
    } while (0)

#define BOARD_I2C_RX(_addr, _data, _size)  \
    do                                     \
    {                                      \
        board_i2c_rx(_addr, _data, _size); \
    } while (0)

#define BOARD_START_SOUND(_freq, _dur)  \
    (void)_freq;                        \
    do                                  \
    {                                   \
        board_start_sound(_freq, _dur); \
    } while (0)

#define BOARD_STOP_SOUND()  \
    do                      \
    {                       \
        board_stop_sound(); \
    } while (0)

#define BOARD_DEBOUNCER_STATE uint8_t

void board_nec_start(NEC_t *handle);
void board_nec_stop(NEC_t *handle);
void board_i2c_tx(uint8_t addr, uint8_t const *data, uint8_t size);
void board_i2c_rx(uint8_t addr, uint8_t *data, uint8_t size);
void board_start_sound(uint16_t freq, uint32_t dur);
void board_stop_sound(void);

#endif /* APPS_BLINK_BOARD_BLUEPILL_INC_BOARD_H_ */
