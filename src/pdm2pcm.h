#ifndef PDM2PCM_H
#define PDM2PCM_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "pdm2pcm_config.h"


#define BYTE_LEFT_MSB 0
#define BYTE_RIGHT_MSB 1

#define PDM_ENDIANNESS_BE 0
#define PDM_ENDIANNESS_LE 1

#define SINC3 0
#define SINC4 1


int pdm2pcm_volume(int vol);
int pdm2pcm_init(int bit_order, int endianess, int sinc);
void pdm2pcm(uint8_t *data_in, int16_t *data_out, const int16_t size);

#endif
