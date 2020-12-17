/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LUT_FILTER_SINC3_4_DEC_8_H__
#define __LUT_FILTER_SINC3_4_DEC_8_H__

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

int LuT_Filter_init(int bitOrder, int endian, int sinc);
void LuT_Filter(uint8_t *PntIn, int16_t *PntOut, uint16_t DataLen);

#endif /* __LUT_FILTER_SINC3_4_DEC_8_H__ */
