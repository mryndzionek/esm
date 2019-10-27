#include "esm/esm.h"
#include "esm/esm_timer.h"

#include "board.h"
#include "ds3231.h"

ESM_THIS_FILE;

typedef struct
{
} rtc_cfg_t;

typedef struct
{
    esm_t esm;
    esm_timer_t timer;
    ds3231_time_t time;
    uint8_t pending;
    rtc_cfg_t const *const cfg;
} rtc_esm_t;

ESM_DEFINE_STATE(sync);
ESM_DEFINE_STATE(running);

static void ds3231_init(void)
{
    uint8_t xferbuf[8] = {DS3231_CONTROL_REG};

    // // Turn on 1Hz RTC SQW output
    BOARD_I2C_TX(DS3231_I2C_ADDRESS, xferbuf, 1);
    BOARD_I2C_RX(DS3231_I2C_ADDRESS, &xferbuf[1], 1);

    xferbuf[1] &= ~(DS3231_CTRL_INTCN | DS3231_CTRL_RS1 | DS3231_CTRL_RS2);
    xferbuf[1] |= DS3231_CTRL_BBSQW;
    BOARD_I2C_TX(DS3231_I2C_ADDRESS, xferbuf, 2);

    // // set time - harcoded for now
    // xferbuf[0] = DS3231_TIME_CAL_REG;
    // xferbuf[1] = dectobcd(0);
    // xferbuf[2] = dectobcd(8);
    // xferbuf[3] = dectobcd(20);
    // xferbuf[4] = dectobcd(7);
    // xferbuf[5] = dectobcd(27);
    // xferbuf[6] = dectobcd(10) + 0x80;
    // xferbuf[7] = dectobcd(19);

    // BOARD_I2C_TX(DS3231_I2C_ADDRESS, xferbuf, 8);
}

static void ds3231_set_time(ds3231_time_t *time)
{
    uint8_t xferbuf[8] = {DS3231_TIME_CAL_REG};

    xferbuf[1] = dectobcd(time->sec);
    xferbuf[2] = dectobcd(time->min);
    xferbuf[3] = dectobcd(time->hour);
    xferbuf[4] = dectobcd(time->wday);
    xferbuf[5] = dectobcd(time->mday);
    xferbuf[6] = dectobcd(time->mon) + 0x80;
    xferbuf[7] = dectobcd(time->year);

    BOARD_I2C_TX(DS3231_I2C_ADDRESS, xferbuf, 8);
}

static void ds3231_get_time(ds3231_time_t *time)
{
    uint8_t xferbuf[8] = {DS3231_CONTROL_REG};
    //read the time
    xferbuf[0] = DS3231_TIME_CAL_REG;
    BOARD_I2C_TX(DS3231_I2C_ADDRESS, xferbuf, 1);
    BOARD_I2C_RX(DS3231_I2C_ADDRESS, &xferbuf[1], 7);

    time->sec = bcdtodec(xferbuf[1]);
    time->min = bcdtodec(xferbuf[2]);
    time->hour = bcdtodec(xferbuf[3]);
    time->wday = bcdtodec(xferbuf[4]);
    time->mday = bcdtodec(xferbuf[5]);
    time->mon = bcdtodec(xferbuf[6] & (0x7F));
    time->year = bcdtodec(xferbuf[7]);
}

static void update_time(ds3231_time_t *time)
{
    time->sec = (time->sec + 1) % 60;
    if (time->sec == 0)
    {
        time->min = (time->min + 1) % 60;
        if (time->min == 0)
        {
            time->hour = (time->hour + 1) % 24;
        }
    }
}

static void dst_adjust(ds3231_time_t *time)
{
    // summer -> winter ?
    if ((time->mon == 10) && (time->mday + 7 > 31) 
    && (time->wday == 7) && (time->hour == 3)
    && (time->min == 0) && (time->sec == 0))
    {
        time->hour--;
        ds3231_set_time(time);
    } else // winter -> summer?
    if ((time->mon == 3) && (time->mday + 7 > 31) 
    && (time->wday == 7) && (time->hour == 2)
    && (time->min == 0) && (time->sec == 0))
    {
        time->hour++;
        ds3231_set_time(time);
    }
}

static void esm_sync_entry(esm_t *const esm)
{
    (void)esm;
    ds3231_init();
}

static void esm_sync_exit(esm_t *const esm)
{
    (void)esm;
}

static void esm_sync_handle(esm_t *const esm, const esm_signal_t *const sig)
{
    rtc_esm_t *self = ESM_CONTAINER_OF(esm, rtc_esm_t, esm);

    switch (sig->type)
    {
    case esm_sig_alarm:
        ds3231_get_time(&self->time);
        dst_adjust(&self->time);
        ESM_TRANSITION(running);
        break;

    default:
        ESM_TRANSITION(unhandled);
        break;
    }
}

static void esm_running_entry(esm_t *const esm)
{
    (void)esm;
}

static void esm_running_exit(esm_t *const esm)
{
    (void)esm;
}

static void esm_running_handle(esm_t *const esm, const esm_signal_t *const sig)
{
    rtc_esm_t *self = ESM_CONTAINER_OF(esm, rtc_esm_t, esm);

    switch (sig->type)
    {
    case esm_sig_alarm:
        update_time(&self->time);
        if ((self->time.sec == 0) && (self->time.min == 0) && (self->time.hour == 0))
        {
            ds3231_get_time(&self->time);
        }
        dst_adjust(&self->time);
        // alarm - hardcoded for now to 04:00
        // Mon to Fri
        if ((self->time.wday < 6) &&
            (self->time.hour == 4) &&
            (self->time.min == 0) &&
            (self->time.sec == 0))
        {
            self->time.alarm = true;
        }
        else
        {
            self->time.alarm = false;
        }
        {
            esm_signal_t s = {
                .type = esm_sig_rtc,
                .params.time = &self->time,
                .sender = NULL,
                .receiver = clock1_esm};
            esm_send_signal(&s);
        }
        break;

    default:
        ESM_TRANSITION(unhandled);
        break;
    }
}

static void esm_rtc_init(esm_t *const esm)
{
    ESM_TRANSITION(sync);
}

static const rtc_cfg_t rtc1_cfg = {};

ESM_REGISTER(rtc, rtc1, esm_gr_none, 2, 0);
