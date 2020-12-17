/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __PDM_H__
#define __PDM_H__

/* Includes -----------------------------------------------------------------------*/
#include <Arduino.h>
#include "pdm_config.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Weak Functions - They must be implemented by the user */
int pdm_low_level_init(void);
int pdm_low_level_deinit(void);
int pdm_low_level_record(void);
int pdm_low_level_stop(void);
int pdm_low_level_pause(void);
int pdm_low_level_resume(void);
void pdm_low_level_copy_half_cplt(uint16_t *pBuf);
void pdm_low_level_copy_cplt(uint16_t *pBuf);

#ifdef __cplusplus
}
#endif


/* Defines ------------------------------------------------------------------------*/

/* Typedefs -----------------------------------------------------------------------*/
/* Microphone Parameter*/
typedef struct {
  uint32_t Device;
  uint32_t SampleRate;
  uint32_t BitsPerSample;
  uint32_t ChannelsNbr;
  uint16_t *pBuff;
  uint32_t State;
  uint32_t DecimatorFactor;
} AudioIn_Ctx_t;


#define PDM_OK      0
#define PDM_ERROR   1

/* Audio In states */
#define AUDIO_IN_STATE_RESET               0U
#define AUDIO_IN_STATE_RECORDING           1U
#define AUDIO_IN_STATE_STOP                2U
#define AUDIO_IN_STATE_PAUSE               3U

/* Class Declaration --------------------------------------------------------------*/

class PDMClass {
  public:
    /* Constructor */
    PDMClass();

    /* Functions */
    int Begin();
    int End();
    int Record(uint16_t *pBuf);
    int Stop();
    int Pause();
    int Resume();
    void GetState(int *status);

    /* Interrupt */
    void onReceive(void(*)(void));

    /* private */
    void DMAHalfCpltCallback(void);
    void DMACpltCallback(void);

  private:

    void (*_onReceive)(void);

    AudioIn_Ctx_t mic;
};

extern PDMClass PDM;

#endif
