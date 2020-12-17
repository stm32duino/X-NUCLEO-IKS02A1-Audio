/* Includes ------------------------------------------------------------------------*/
#include <PDM.h>

/* Define---------------------------------------------------------------------------*/

/* Class Implementation -------------------------------------------------------------------------------------------------------------------------------------------------------- */
/* Constructor */
PDMClass::PDMClass()
{
}

/* Methods */

/**
 *  @brief Initialize microphone and I2S/SAI parameter
 *  @param None
 *  @retval 1 if success, 0 otherwise
 */
int PDMClass::Begin()
{
  int ret = PDM_OK;

  /* Context */
  mic.Device = 0;
  mic.ChannelsNbr = AUDIO_IN_CHANNELS;
  mic.SampleRate = AUDIO_IN_SAMPLING_FREQUENCY; /*16k*/
  mic.BitsPerSample = AUDIO_IN_BITPERSAMPLE;
  mic.State = AUDIO_IN_STATE_RESET;
  mic.DecimatorFactor = AUDIO_IN_DECIMATOR_FACTOR;

  ret = pdm_low_level_init();

  // Update State to STOP
  mic.State = AUDIO_IN_STATE_STOP;

  return ret;
}

/**
 *  @brief Deinizialize microphone and I2S/SAI parameter
 *  @param None
 *  @retval 1 if success, 0 otherwise
 */
int PDMClass::End()
{
  int ret = PDM_OK;

  ret = pdm_low_level_deinit();

  mic.State = AUDIO_IN_STATE_RESET;

  return ret;
}

/**
 *  @brief  DMA Half Callback
 *  @param None
 *  @retval None
 */
void PDMClass::DMAHalfCpltCallback(void)
{
  pdm_low_level_copy_half_cplt(mic.pBuff);

  _onReceive();
}

/**
 *  @brief  DMA Complete Callback
 *  @param None
 *  @retval None
 */
void PDMClass::DMACpltCallback(void)
{
  pdm_low_level_copy_cplt(mic.pBuff);

  _onReceive();
}

/**
 *  @brief Start recording
 *  @param pBuf: pointer to a buffer to store data
 *  @retval 1 if success, 0 otherwise
 */
int PDMClass::Record(uint16_t *pBuf)
{
  int ret = PDM_OK;

  mic.pBuff = pBuf;

  ret = pdm_low_level_record();

  /* Update State */
  mic.State = AUDIO_IN_STATE_RECORDING;

  return ret;
}

/**
 *  @brief Stop recording
 *  @param None
 *  @retval 1 if success, 0 otherwise
 */
int PDMClass::Stop()
{
  int ret = PDM_OK;

  ret = pdm_low_level_stop();

  /* Update AUDIO IN state */
  mic.State = AUDIO_IN_STATE_STOP;
  /* Return Status */
  return ret;
}

/**
 *  @brief Pause recording
 *  @param None
 *  @retval 1 if success, 0 otherwise
 */
int PDMClass::Pause()
{
  int ret = PDM_OK;

  ret = pdm_low_level_pause();

  /* Update AUDIO IN state */
  mic.State = AUDIO_IN_STATE_PAUSE;

  /* Return Status */
  return ret;
}

/**
 *  @brief Resume recording
 *  @param None
 *  @retval 1 if success, 0 otherwise
 */
int PDMClass::Resume()
{
  int ret = PDM_OK;

  ret = pdm_low_level_resume();

  /* Update AUDIO IN state */
  mic.State = AUDIO_IN_STATE_RECORDING;

  /* Return Status */
  return ret;
}

/**
 *  @brief Star recording
 *  @param status: pointer where to save the status
 *  @retval None
 */
void PDMClass::GetState(int *status)
{
  *status = mic.State;
}

/**
 *  @brief Set user function
 *  @param function: pointer to a function
 *  @retval None
 */
void PDMClass::onReceive(void(*function)(void))
{
  _onReceive = function;
}

#ifdef __cplusplus
extern "C" {
#endif

/* Low Level Functions: This functions are empty and must be implemented in the platform dependent low level driver */
__attribute__((weak)) int pdm_low_level_init(void)
{
  return PDM_ERROR;
}

__attribute__((weak)) int pdm_low_level_deinit(void)
{
  return PDM_ERROR;
}

__attribute__((weak)) int pdm_low_level_record(void)
{
  return PDM_ERROR;
}

__attribute__((weak)) int pdm_low_level_stop(void)
{
  return PDM_ERROR;
}

__attribute__((weak)) int pdm_low_level_pause(void)
{
  return PDM_ERROR;
}

__attribute__((weak)) int pdm_low_level_resume(void)
{
  return PDM_ERROR;
}

__attribute__((weak)) void pdm_low_level_copy_half_cplt(uint16_t *pBuf)
{
  (void)pBuf;
}

__attribute__((weak)) void pdm_low_level_copy_cplt(uint16_t *pBuf)
{
  (void)pBuf;
}

#ifdef __cplusplus
}
#endif


PDMClass PDM;
