#ifndef _SK6812_H_
#define _SK6812_H_

#include <stdint.h>

void sk6812_set_rgb(uint16_t n, uint8_t r, uint8_t g, uint8_t b);
void sk6812_set_color(uint16_t n, uint32_t c);
void sk6812_set_brightness(uint8_t b);
void sk6812_show(void);
void sk6812_clear(void);
void sk6812_set_brightness(uint8_t b);

#endif // _SK6812_H_