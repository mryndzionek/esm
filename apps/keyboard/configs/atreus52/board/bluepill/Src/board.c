#include "board.h"
#include "esm/esm.h"

typedef struct
{
	GPIO_TypeDef *port;
	uint16_t pin;
} pin_desc_t;

const uint32_t board_heat_colormap[256] =
    {
        0x0,
        0x70000,
        0xD0000,
        0x120000,
        0x160000,
        0x190000,
        0x1C0000,
        0x1F0000,
        0x220000,
        0x240000,
        0x260000,
        0x280000,
        0x2A0000,
        0x2C0000,
        0x2E0000,
        0x300000,
        0x320000,
        0x330000,
        0x350000,
        0x360000,
        0x380000,
        0x390000,
        0x3B0000,
        0x3C0000,
        0x3E0000,
        0x3F0000,
        0x400000,
        0x420100,
        0x430100,
        0x450100,
        0x460100,
        0x480100,
        0x490100,
        0x4B0100,
        0x4C0100,
        0x4E0100,
        0x4F0100,
        0x510100,
        0x520100,
        0x540100,
        0x550100,
        0x570100,
        0x580100,
        0x5A0100,
        0x5B0100,
        0x5D0100,
        0x5E0100,
        0x600100,
        0x610100,
        0x630100,
        0x640100,
        0x660100,
        0x670100,
        0x690100,
        0x6B0200,
        0x6C0200,
        0x6E0200,
        0x6F0200,
        0x710200,
        0x720200,
        0x740200,
        0x760200,
        0x770200,
        0x790200,
        0x7A0200,
        0x7C0200,
        0x7E0200,
        0x7F0200,
        0x810200,
        0x830200,
        0x840300,
        0x860300,
        0x870300,
        0x890300,
        0x8B0300,
        0x8C0300,
        0x8E0300,
        0x900300,
        0x910300,
        0x930300,
        0x950300,
        0x960400,
        0x980400,
        0x9A0400,
        0x9B0400,
        0x9D0400,
        0x9F0400,
        0xA00400,
        0xA20400,
        0xA40400,
        0xA50500,
        0xA70500,
        0xA90500,
        0xAB0500,
        0xAC0500,
        0xAE0500,
        0xB00600,
        0xB10600,
        0xB30600,
        0xB50600,
        0xB70600,
        0xB80600,
        0xBA0700,
        0xBC0700,
        0xBD0700,
        0xBF0700,
        0xC10700,
        0xC30800,
        0xC40800,
        0xC60800,
        0xC80800,
        0xCA0900,
        0xCB0900,
        0xCD0900,
        0xCF0900,
        0xD10A00,
        0xD20A00,
        0xD40A00,
        0xD60B00,
        0xD80B00,
        0xD90C00,
        0xDB0C00,
        0xDD0C00,
        0xDF0D00,
        0xE00D00,
        0xE20E00,
        0xE40F00,
        0xE60F00,
        0xE71000,
        0xE91100,
        0xEB1200,
        0xEC1300,
        0xEE1500,
        0xEF1800,
        0xF01A00,
        0xF11D00,
        0xF22100,
        0xF32400,
        0xF42700,
        0xF42A00,
        0xF52E00,
        0xF63100,
        0xF63400,
        0xF73700,
        0xF73A00,
        0xF73D00,
        0xF83F00,
        0xF84200,
        0xF84500,
        0xF94800,
        0xF94A00,
        0xF94D00,
        0xF94F00,
        0xFA5200,
        0xFA5400,
        0xFA5600,
        0xFA5900,
        0xFA5B00,
        0xFB5D00,
        0xFB6000,
        0xFB6200,
        0xFB6400,
        0xFB6600,
        0xFB6800,
        0xFC6A00,
        0xFC6C00,
        0xFC6E00,
        0xFC7000,
        0xFC7200,
        0xFC7400,
        0xFC7600,
        0xFC7800,
        0xFC7A00,
        0xFC7C00,
        0xFD7E00,
        0xFD8000,
        0xFD8200,
        0xFD8300,
        0xFD8500,
        0xFD8700,
        0xFD8900,
        0xFD8B00,
        0xFD8C00,
        0xFD8E00,
        0xFD9000,
        0xFD9200,
        0xFD9300,
        0xFE9500,
        0xFE9700,
        0xFE9900,
        0xFE9A00,
        0xFE9C00,
        0xFE9E00,
        0xFE9F00,
        0xFEA100,
        0xFEA300,
        0xFEA400,
        0xFEA600,
        0xFEA800,
        0xFEA900,
        0xFEAB00,
        0xFEAD00,
        0xFEAE00,
        0xFEB000,
        0xFEB100,
        0xFEB300,
        0xFEB500,
        0xFEB600,
        0xFEB800,
        0xFEB900,
        0xFEBB00,
        0xFEBD00,
        0xFFBE00,
        0xFFC000,
        0xFFC100,
        0xFFC300,
        0xFFC400,
        0xFFC600,
        0xFFC700,
        0xFFC900,
        0xFFCB00,
        0xFFCC00,
        0xFFCE00,
        0xFFCF00,
        0xFFD100,
        0xFFD200,
        0xFFD400,
        0xFFD500,
        0xFFD700,
        0xFFD800,
        0xFFDA00,
        0xFFDB00,
        0xFFDD00,
        0xFFDE00,
        0xFFE000,
        0xFFE100,
        0xFFE300,
        0xFFE400,
        0xFFE600,
        0xFFE700,
        0xFFE900,
        0xFFEA00,
        0xFFEC00,
        0xFFED00,
        0xFFEF00,
        0xFFF000,
        0xFFF200,
        0xFFF300,
        0xFFF500,
        0xFFF600,
        0xFFF800,
        0xFFF900,
        0xFFFB00,
        0xFFFC00,
        0xFFFE00,
        0xFFFF00,
};

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

// this function is specific to the LED strip layout
void board_ledpos_to_xy(uint8_t p, uint8_t *xp, uint8_t *yp)
{
#define DX (N_COLS / 2)
#define DY (N_ROWS / 3)
#define DL (SK6812_LEDS_NUM / 4)

    switch (p)
    {
    case 0 ... DL - 1:
        *xp = DX + (p * DX) / DL;
        *yp = DY;
        break;

    case DL ...(3 * DL) - 1:
        *xp = (2 * DX) - (((p - DL) * 2 * DX) / (2 * DL));
        *yp = 2 * DY;
        break;

    case (3 * DL)...(4 * DL) - 1:
        *xp = ((p - (3 * DL)) * DX) / DL;
        *yp = DY;
        break;

    default:
        ESM_ASSERT(0);
    }

#undef DL
#undef DY
#undef DX
}
