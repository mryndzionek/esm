#ifndef INCLUDE_ESM_PLATFORM_H_
#define INCLUDE_ESM_PLATFORM_H_

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include "stm32f1xx_hal.h"

extern UART_HandleTypeDef huart3;
uint16_t platform_rnd(uint16_t range);

#define ESM_PRINTF(_format, _args ... )

#define ESM_RESET() {while(1) {}}

#define ESM_BUG_REPORT( _cond, _format, _args ... )

#define ESM_ASSERT(_cond) do { if(!(_cond)) { \
		ESM_BUG_REPORT(_cond, ""); \
		ESM_RESET();}} while(0)

#define ESM_ASSERT_MSG( _cond, _format, _args ... ) \
		do { if(!(_cond)) { ESM_BUG_REPORT( _cond, _format, ##_args ); \
		ESM_RESET(); } } while(0)

#define ESM_INIT do { \
		} while(0)

#define ESM_TICKS_PER_SEC	(1000UL)
#define ESM_WAIT() do { \
		HAL_GPIO_WritePin(IDLE_LED_GPIO_Port, IDLE_LED_Pin, GPIO_PIN_SET); \
		ESM_CRITICAL_ENTER(); \
		for(prio = _ESM_MAX_PRIO - 1; prio >= 0; prio--) \
		{ \
		   if(!esm_list_empty(&esm_signals[prio])) { \
			   break; \
		   } \
		} \
		if(prio < 0) { \
		   HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_STOPENTRY_WFI); \
		} \
		ESM_CRITICAL_EXIT(); \
		HAL_GPIO_WritePin(IDLE_LED_GPIO_Port, IDLE_LED_Pin, GPIO_PIN_RESET); \
} while(0)

#define ESM_RANDOM(_num) platform_rnd(_num)

#define ESM_CRITICAL_ENTER() __disable_irq()
#define ESM_CRITICAL_EXIT() __enable_irq()

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

#define ESM_TRACE(_p_esm, _action, ...) \
		ESM_TRACE_##_action(_p_esm, __VA_ARGS__)

#define ESM_IDLE() do { \
		} while(0)

#define ESM_TRACE_BUF_SIZE		(256)
#define ESM_TRACE_CHUNK_SIZE	(16)
#define ESM_TRACE_OUT(_data, _size) do { \
		HAL_StatusTypeDef r = HAL_UART_Transmit_IT(&huart3, _data, _size); \
		ESM_ASSERT(r == HAL_OK); \
} while(0)

#endif /* INCLUDE_ESM_PLATFORM_H_ */
