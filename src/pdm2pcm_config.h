#ifndef __PDM2PCM_CONFIG_H__
#define __PDM2PCM_CONFIG_H__

#include "pdm_config.h"

/* Number of PDM samples being processed at each iteration. Corresponds to 1ms PDM audio */
#define BLOCK_SIZE AUDIO_IN_FREQ

#define DEC_CIC_FACTOR 8  // First filter Decimation factor

#define N_DATA_CIC_DEC (BLOCK_SIZE/DEC_CIC_FACTOR)

#define DEC_OUT_FACTOR 10  // Second filter Decimation factor

#define FIR_DELAY 4// ceil(gd_cic_1(1)/8) + ceil(gd_cic_2(1)/10) where 8 and 10 are the decimation factors of two filter cascade

#endif /* __PDM2PCM_CONFIG_H__ */
