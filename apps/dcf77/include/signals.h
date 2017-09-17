#ifndef INCLUDE_SIGNALS_H_
#define INCLUDE_SIGNALS_H_

#define ESM_SIGNALS \
		ESM_SIGNAL(tmout) \
		ESM_SIGNAL(dcf77_data)

#define ESM_IDS \
		ESM_ID(phase) \
		ESM_ID(led) \
		ESM_ID(debug)

typedef enum
{
	dcf77_sync_mark = 0,
	dcf77_undefined,
	dcf77_zero,
	dcf77_one,
} dcf77_data_e;

typedef struct
{
	union
	{
		struct {
			uint16_t tick;
			uint8_t bin;
		} tick;
		struct {
			dcf77_data_e data;
		} dcf77_data;
	};
} esm_sig_params_t;

#endif /* INCLUDE_SIGNALS_H_ */
