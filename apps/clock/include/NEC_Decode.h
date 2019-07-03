/*
 * NEC_Decode.h
 *
 *  Created on: Mar 9, 2016
 *      Author: peter
 */

#ifndef INC_NEC_DECODE_H_
#define INC_NEC_DECODE_H_

#include <stdint.h>
#include "stm32f1xx_hal.h"

typedef enum
{
    NEC_NOT_EXTENDED,
    NEC_EXTENDED
} NEC_TYPE;

typedef enum
{
    NEC_INIT,
    NEC_AGC_OK,
    NEC_AGC_FAIL,
    NEC_FAIL,
    NEC_OK
} NEC_STATE;

typedef struct
{
    int rawTimerData[32];
    uint8_t decoded[4];

    NEC_STATE state;

    TIM_HandleTypeDef *timerHandle;

    uint32_t timerChannel;
    HAL_TIM_ActiveChannel timerChannelActive;

    uint16_t timingBitBoundary;
    uint16_t timingAgcBoundary;
    NEC_TYPE type;
} NEC;

void NEC_TIM_IC_CaptureCallback(NEC *handle);

void NEC_Read(NEC *handle);

#endif /* INC_NEC_DECODE_H_ */
