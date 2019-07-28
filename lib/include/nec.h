#ifndef _NEC_H_
#define _NEC_H_

#include <stdint.h>
#include "board.h"

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
} NEC_STATE;

typedef struct _NEC_HW_CTX NEC_HW_CTX_t;

typedef struct
{
    int rawTimerData[32];
    uint8_t decoded[4];

    NEC_STATE state;

    const NEC_HW_CTX_t * const hwctx;

    uint16_t timingBitBoundary;
    uint16_t timingAgcBoundary;
    NEC_TYPE type;
} NEC_t;

#endif // _NEC_H_
