#include "iir_hp.h"

int16_t iir_state;
int64_t iir_prev;

#define IIR_DEN -32113 // --> iir filter with num [1 -1] ; den=[1 -0.98]  ; -32113 = round(-0.98*2^15)

void iir_hp_init(void)
{
  iir_prev = 0;
  iir_state = 0;

  return;
}

void iir_hp(int16_t *data_out_decim, int16_t data_l)
{
  int64_t iir_out, fir_out_tmp;
  int16_t i;

  for (i = 0; i < data_l; i++) {
    fir_out_tmp = ((int64_t)data_out_decim[i]) << 15;

    // ====== First order high-pass iir filter for DC suppression ================
    iir_out = (fir_out_tmp - iir_prev) - ((int64_t)(IIR_DEN) * iir_state);
    // reduce to int16_t
    data_out_decim[i]   = (int16_t)(iir_out >> 15);

    iir_prev = fir_out_tmp;
    iir_state = data_out_decim[i];
  }

  return;
}
