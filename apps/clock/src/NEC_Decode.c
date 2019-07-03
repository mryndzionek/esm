/*
 * NEC_Decode.c
 *
 *  Created on: Mar 9, 2016
 *      Author: peter
 */

#include "NEC_Decode.h"
#include "esm/esm.h"

void NEC_TIM_IC_CaptureCallback(NEC *handle)
{
    static uint8_t ncmd = 0;

    if (handle->state == NEC_INIT)
    {
        HAL_TIM_IC_Stop_DMA(handle->timerHandle, handle->timerChannel);

        if (handle->rawTimerData[1] < handle->timingAgcBoundary)
        {
            handle->state = NEC_OK;
            if (ncmd)
            {
                esm_signal_t sig = {
                    .type = esm_sig_remote,
                    .params.keycode = ncmd,
                    .sender = NULL,
                    .receiver = clock1_esm};
                esm_send_signal(&sig);
            }
            NEC_Read(handle);
        }
        else
        {
            handle->state = NEC_AGC_OK;
            ncmd = 0;
            HAL_TIM_IC_Start_DMA(handle->timerHandle, handle->timerChannel,
                                 (uint32_t *)handle->rawTimerData, 32);
        }
    }
    else if (handle->state == NEC_AGC_OK)
    {
        HAL_TIM_IC_Stop_DMA(handle->timerHandle, handle->timerChannel);

        for (int pos = 0; pos < 32; pos++)
        {
            int time = handle->rawTimerData[pos];
            if (time > handle->timingBitBoundary)
            {
                handle->decoded[pos / 8] |= 1 << (pos % 8);
            }
            else
            {
                handle->decoded[pos / 8] &= ~(1 << (pos % 8));
            }
        }

        uint8_t valid = 1;

        uint8_t naddr = ~handle->decoded[0];
        ncmd = ~handle->decoded[2];

        if (handle->type == NEC_NOT_EXTENDED && handle->decoded[1] != naddr)
            valid = 0;
        if (handle->decoded[3] != ncmd)
            valid = 0;

        handle->state = NEC_OK;

        if (valid)
        {
            esm_signal_t sig = {
                .type = esm_sig_remote,
                .params.keycode = ncmd,
                .sender = NULL,
                .receiver = clock1_esm};
            esm_send_signal(&sig);
        }
        NEC_Read(handle);
    }
}

void NEC_Read(NEC *handle)
{
    handle->state = NEC_INIT;
    HAL_TIM_IC_Start_DMA(handle->timerHandle, handle->timerChannel,
                         (uint32_t *)handle->rawTimerData, 2);
}
