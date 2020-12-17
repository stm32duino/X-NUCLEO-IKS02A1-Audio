#ifndef __IIR_HP_H__
#define __IIR_HP_H__

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

void iir_hp_init(void);
void iir_hp(int16_t *data_out_decim, int16_t data_l);

#endif /* __IIR_HP_H__ */
