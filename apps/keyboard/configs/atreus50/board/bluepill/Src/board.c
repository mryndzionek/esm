#include "board.h"
#include "esm/esm.h"

typedef struct
{
	GPIO_TypeDef *port;
	uint16_t pin;
} pin_desc_t;

static const pin_desc_t cols[N_COLS] = {
	{COL1_OUT_GPIO_Port, COL1_OUT_Pin},
	{COL2_OUT_GPIO_Port, COL2_OUT_Pin},
	{COL3_OUT_GPIO_Port, COL3_OUT_Pin},
	{COL4_OUT_GPIO_Port, COL4_OUT_Pin},
	{COL5_OUT_GPIO_Port, COL5_OUT_Pin},
	{COL6_OUT_GPIO_Port, COL6_OUT_Pin},
	{COL7_OUT_GPIO_Port, COL7_OUT_Pin},
	{COL8_OUT_GPIO_Port, COL8_OUT_Pin},
	{COL9_OUT_GPIO_Port, COL9_OUT_Pin},
	{COL10_OUT_GPIO_Port, COL10_OUT_Pin},
	{COL11_OUT_GPIO_Port, COL11_OUT_Pin},
	{COL12_OUT_GPIO_Port, COL12_OUT_Pin},
	{COL13_OUT_GPIO_Port, COL13_OUT_Pin},
};

static const pin_desc_t rows[N_ROWS] = {
	{ROW1_IN_GPIO_Port, ROW1_IN_Pin},
	{ROW2_IN_GPIO_Port, ROW2_IN_Pin},
	{ROW3_IN_GPIO_Port, ROW3_IN_Pin},
	{ROW4_IN_GPIO_Port, ROW4_IN_Pin},
};

bool board_usb_send(uint8_t *data, uint16_t len)
{
	if (hUsbDeviceFS.dev_state != USBD_STATE_CONFIGURED)
	{
		return false;
	}
	USBD_HID_SendReport(&hUsbDeviceFS, data, len);
	
	return true;
}

void board_read_matrix(bool (*const matrix)[N_COLS][N_ROWS])
{
	for (uint8_t j = 0; j < N_COLS; j++)
	{
		HAL_GPIO_WritePin(cols[j].port, cols[j].pin, GPIO_PIN_SET);
		for (uint8_t i = 0; i < N_ROWS; i++)
		{
			if (HAL_GPIO_ReadPin(rows[i].port, rows[i].pin) == GPIO_PIN_SET)
			{
				(*matrix)[j][i] = true;
			}
			else
			{
				(*matrix)[j][i] = false;
			}
		}
		HAL_GPIO_WritePin(cols[j].port, cols[j].pin, GPIO_PIN_RESET);
	}
}