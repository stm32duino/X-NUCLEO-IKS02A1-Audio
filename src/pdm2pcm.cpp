#include "pdm2pcm.h"
#include "LuT_Filter_SINC3_4_DEC_8.h"
#include "iir_hp.h"
#include "arm_math.h"

#define N_TAPS_FIR_DEC 37  // SINC4 DEC 10

static int16_t fir_taps[7][N_TAPS_FIR_DEC] = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {7, 26, 66, 131, 229, 367, 551, 786, 1081, 1442, 1848, 2281, 2720, 3146, 3539, 3880, 4148, 4325, 4391, 4325, 4148, 3880, 3539, 3146, 2720, 2281, 1848, 1442, 1081, 786, 551, 367, 229, 131, 66, 26, 7},
  {13, 52, 131, 262, 459, 734, 1101, 1573, 2163, 2884, 3696, 4561, 5439, 6291, 7078, 7759, 8297, 8651, 8782, 8651, 8297, 7759, 7078, 6291, 5439, 4561, 3696, 2884, 2163, 1573, 1101, 734, 459, 262, 131, 52, 13},
  {20, 79, 197, 393, 688, 1101, 1652, 2359, 3244, 4325, 5544, 6842, 8159, 9437, 10617, 11639, 12445, 12976, 13173, 12976, 12445, 11639, 10617, 9437, 8159, 6842, 5544, 4325, 3244, 2359, 1652, 1101, 688, 393, 197, 79, 20},
  {26, 105, 262, 524, 918, 1468, 2202, 3146, 4325, 5767, 7392, 9123, 10879, 12583, 14156, 15519, 16594, 17302, 17564, 17302, 16594, 15519, 14156, 12583, 10879, 9123, 7392, 5767, 4325, 3146, 2202, 1468, 918, 524, 262, 105, 26},
  {33, 131, 328, 655, 1147, 1835, 2753, 3932, 5407, 7209, 9241, 11403, 13599, 15729, 17695, 19399, 20742, 21627, 21955, 21627, 20742, 19399, 17695, 15729, 13599, 11403, 9241, 7209, 5407, 3932, 2753, 1835, 1147, 655, 328, 131, 33},
  {39, 157, 393, 786, 1376, 2202, 3303, 4719, 6488, 8651, 11089, 13684, 16318, 18874, 21234, 23278, 24891, 25952, 26345, 25952, 24891, 23278, 21234, 18874, 16318, 13684, 11089, 8651, 6488, 4719, 3303, 2202, 1376, 786, 393, 157, 39}
};
int32_t volume = 5;

int16_t FirState[((BLOCK_SIZE / DEC_CIC_FACTOR) + N_TAPS_FIR_DEC - 1)];
int16_t data_out_decim[N_DATA_CIC_DEC / DEC_OUT_FACTOR];
arm_fir_decimate_instance_q15 fir_decim_S;
int16_t data_in_cic_decim[N_DATA_CIC_DEC];
uint32_t delay_counter;
int16_t delay_buf[FIR_DELAY];

int pdm2pcm_volume(int vol)
{
  int ret_val = 1;

  if (vol >= 0 && vol <= 6) {
    volume = vol;
    ret_val = 0;
  }

  memset((void *)&fir_decim_S, 0x0, sizeof(arm_fir_decimate_instance_q15));
  memset((void *)FirState, 0x0, sizeof(FirState));
  arm_fir_decimate_init_q15(&fir_decim_S, N_TAPS_FIR_DEC, DEC_OUT_FACTOR, fir_taps[volume], FirState, (BLOCK_SIZE / DEC_CIC_FACTOR));
  memset(delay_buf, 0, sizeof(int16_t)*FIR_DELAY);
  delay_counter = 0;

  return ret_val;
}

int pdm2pcm_init(int bit_order, int endianess, int sinc)
{
  int ret_val = 0;

  arm_fir_decimate_init_q15(&fir_decim_S, N_TAPS_FIR_DEC, DEC_OUT_FACTOR, fir_taps[volume], FirState, (BLOCK_SIZE / DEC_CIC_FACTOR));

  iir_hp_init();

  ret_val = LuT_Filter_init(bit_order, endianess, sinc);

  memset(delay_buf, 0, sizeof(int16_t)*FIR_DELAY);

  delay_counter = 0;

  return ret_val;
}

void pdm2pcm(uint8_t *data_in, int16_t *data_out, int16_t size)
{
  int16_t k, cic_dec_l, out_dec_l, size_dec;

  cic_dec_l = size / DEC_CIC_FACTOR;
  out_dec_l = cic_dec_l / DEC_OUT_FACTOR;

  size_dec = size >> 3;
  LuT_Filter(data_in, data_in_cic_decim, size_dec);

  // ****** Second cic-decim, high-pass iir and Group-delay compensation **********************//
  arm_fir_decimate_q15(&fir_decim_S, data_in_cic_decim, data_out_decim, (BLOCK_SIZE / DEC_CIC_FACTOR));

  iir_hp(data_out_decim, out_dec_l);

  for (k = 0; k < out_dec_l; k++) {
    //============================================================================
    *(data_out + k) = delay_buf[delay_counter]; // compensate the group delay of filter cascade

    delay_buf[delay_counter++] = data_out_decim[k];

    if (delay_counter == FIR_DELAY) {
      delay_counter = 0;
    }
  }

  return;
}
