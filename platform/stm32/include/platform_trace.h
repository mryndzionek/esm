#ifndef INCLUDE_ESM_PLATFORM_TRACE_H_
#define INCLUDE_ESM_PLATFORM_TRACE_H_

#include "stm32f1xx_hal.h"
#include "main.h"

#ifndef ESM_TRACE_DISABLE

#ifndef ESM_TRACE_UART
#define ESM_TRACE_UART (huart3)
#endif

extern UART_HandleTypeDef ESM_TRACE_UART;

#define ESM_TRACE_BUF_SIZE		(256)
#define ESM_TRACE_CHUNK_SIZE	(32)
#define ESM_TRACE_OUT(_data, _size) do { \
		HAL_StatusTypeDef r = HAL_UART_Transmit_IT(&ESM_TRACE_UART, _data, _size); \
		ESM_ASSERT(r == HAL_OK); \
} while(0)

#else

#define ESM_TRACE_BUF_SIZE		(256)
#define ESM_TRACE_CHUNK_SIZE	(32)
#define ESM_TRACE_OUT(_data, _size) do { \
} while(0)

#endif

#endif /* INCLUDE_ESM_PLATFORM_TRACE_H_ */
