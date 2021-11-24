/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __PDM_CONFIG_H__
#define __PDM_CONFIG_H__

#include "app_config.h"

/* IKS02A1 Audio Device Configuration */
#define AUDIO_IN_FREQ                    1280  /*KHz*/
#define MAX_AUDIO_IN_CHANNEL_NBR_PER_IF     1
#define MAX_AUDIO_IN_CHANNEL_NBR_TOTAL      1
#define AUDIO_IN_CHANNELS                   1
#define AUDIO_IN_BITPERSAMPLE              16
#define AUDIO_IN_SAMPLING_FREQUENCY     16000
#define AUDIO_IN_DECIMATOR_FACTOR ((AUDIO_IN_FREQ * 1000) / AUDIO_IN_SAMPLING_FREQUENCY)

#endif /* __PDM_CONFIG_H__ */
