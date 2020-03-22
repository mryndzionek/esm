#ifndef APPS_BLINK_BOARD_BLUEPILL_INC_BOARD_H_
#define APPS_BLINK_BOARD_BLUEPILL_INC_BOARD_H_

#include <stdint.h>
#include "nec.h"

#define N_ROWS (1)
#define N_COLS (SK6812_LEDS_NUM)

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

#define board_backlight_show()       \
    {                                \
        esm_signal_t s = {           \
            .type = esm_sig_alarm,   \
            .sender = NULL,          \
            .receiver = strip1_esm}; \
        esm_send_signal(&s);         \
    }

extern const uint32_t board_heat_colormap[256];

void board_nec_start(NEC_t *handle);
void board_nec_stop(NEC_t *handle);
void board_i2c_tx(uint8_t addr, uint8_t const *data, uint8_t size);
void board_i2c_rx(uint8_t addr, uint8_t *data, uint8_t size);
void board_start_sound(uint16_t freq, uint32_t dur);
void board_stop_sound(void);
void board_ledpos_to_xy(uint8_t p, uint8_t *xp, uint8_t *yp);

#endif /* APPS_BLINK_BOARD_BLUEPILL_INC_BOARD_H_ */
