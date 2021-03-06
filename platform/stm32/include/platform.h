#ifndef INCLUDE_ESM_PLATFORM_H_
#define INCLUDE_ESM_PLATFORM_H_

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include "stm32f1xx_hal.h"
#include "main.h"

#include "platform_trace.h"

uint16_t platform_rnd(uint16_t range);

#define ESM_PRINTF(_format, _args ... )

#define ESM_INIT do { \
		} while(0)

#define ESM_TICKS_PER_SEC	(1000UL)
#define ESM_WAIT() do { \
      do { \
         HAL_GPIO_WritePin(IDLE_LED_GPIO_Port, IDLE_LED_Pin, GPIO_PIN_SET); \
         ESM_CRITICAL_ENTER(); \
         if(!prio_mask) { \
            HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_STOPENTRY_WFI); \
         } \
         ESM_CRITICAL_EXIT(); \
         HAL_GPIO_WritePin(IDLE_LED_GPIO_Port, IDLE_LED_Pin, GPIO_PIN_RESET); \
      }while(!prio_mask); \
} while(0)

#define ESM_RANDOM(_num) platform_rnd(_num)

#define ESM_CRITICAL_ENTER() asm volatile ("cpsid i" : : : "memory")
#define ESM_CRITICAL_EXIT()  asm volatile ("cpsie i" : : : "memory")

#define ESM_TRACE_init(_p_esm, ...) do { \
		trace_init(_p_esm->cfg->id); \
} while (0)

#define ESM_TRACE_enter(_p_esm, _name, ...) do { \
} while (0)

#define ESM_TRACE_exit(_p_esm, _name, ...) do { \
} while (0)

#define ESM_TRACE_init(_p_esm, ...) do { \
		trace_init(_p_esm->cfg->id); \
} while (0)

#define ESM_TRACE_trans(_p_esm, _sig, ...) do { \
		trace_trans(_p_esm->cfg->id, _sig->type, _p_esm->curr_state->name,  _p_esm->next_state->name); \
} while (0)

#define ESM_TRACE_receive(_p_esm, _sig, ...) do { \
		trace_receive(_p_esm->cfg->id, _sig->type, _p_esm->curr_state->name); \
} while (0)

#ifndef ESM_TRACE_DISABLE
#define ESM_TRACE(_p_esm, _action, ...) \
   if (!_p_esm->trace_off) \
   { \
      ESM_TRACE_##_action(_p_esm, __VA_ARGS__); \
   }
#else
#define ESM_TRACE(_p_esm, _action, ...)
#endif

#define ESM_IDLE() do { \
		} while(0)

#endif /* INCLUDE_ESM_PLATFORM_H_ */
