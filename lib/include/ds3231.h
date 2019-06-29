#ifndef _DS3231_H_
#define _DS3231_H_

#define DS3231_I2C_ADDRESS (0x68 << 1)
#define DS3231_TIME_CAL_REG (0x00)
#define DS3231_CONTROL_REG (0x0e)
#define DS3231_CTRL_INTCN (1UL << 2)
#define DS3231_CTRL_RS1 (1UL << 3)
#define DS3231_CTRL_RS2 (1UL << 4)
#define DS3231_CTRL_BBSQW (1UL << 6)

typedef struct
{
    uint8_t sec;
    uint8_t min;
    uint8_t hour;
    uint8_t mday;
    uint8_t mon;
    int16_t year;
    uint8_t wday;
    uint8_t yday;
    uint8_t year_s;
    uint32_t unixtime;
} ds3231_time_t;

inline uint8_t bcdtodec(const uint8_t val)
{
	return ((val / 16 * 10) + (val % 16));
}

inline uint8_t dectobcd(const uint8_t val)
{
    return ((val / 10 * 16) + (val % 10));
}

#endif // _DS3231_H_
