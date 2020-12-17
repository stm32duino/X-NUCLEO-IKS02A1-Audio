#if defined(ARDUINO_NUCLEO_L476RG)

#include "PDM.h"

#ifdef __cplusplus
extern "C" {
#endif

uint32_t SAI1_client = 0;
SAI_HandleTypeDef hsai_BlockB1;
DMA_HandleTypeDef hdma_sai1_b;
uint16_t SAI_InternalBuffer[((AUDIO_IN_FREQ / 8) * MAX_AUDIO_IN_CHANNEL_NBR_PER_IF * N_MS_PER_INTERRUPT)];
uint32_t SAI_InternalBufferSize = (((AUDIO_IN_FREQ / 8) * MAX_AUDIO_IN_CHANNEL_NBR_PER_IF *N_MS_PER_INTERRUPT) * 2);

static void MX_DMA_Init(void);
static int MX_SAI1_Init(void);

/* Low Level Functions implementation */

/**
 *  @brief PDM initialization
 *  @param None
 *  @retval 1 if success, 0 otherwise
 */
int pdm_low_level_init(void)
{
  int ret = PDM_OK;
  MX_DMA_Init();
  ret = MX_SAI1_Init();
  return ret;
}

/**
 *  @brief PDM deinitialization
  * @param None
 *  @retval 1 if success, 0 otherwise
 */
int pdm_low_level_deinit(void)
{
  if (HAL_SAI_DeInit(&hsai_BlockB1) != HAL_OK) {
    return PDM_ERROR;
  }

  return PDM_OK;
}

/**
 *  @brief Start recording
  * @param None
 *  @retval 1 if success, 0 otherwise
 */
int pdm_low_level_record(void)
{
  if (HAL_SAI_Receive_DMA(&hsai_BlockB1, (uint8_t *)SAI_InternalBuffer, (uint16_t)SAI_InternalBufferSize / 2U) != HAL_OK) {
    return PDM_ERROR;
  }

  return PDM_OK;
}

/**
 *  @brief Stop recording
  * @param None
 *  @retval 1 if success, 0 otherwise
 */
int pdm_low_level_stop(void)
{
  if (HAL_SAI_DMAStop(&hsai_BlockB1) != HAL_OK) {
    return PDM_ERROR;
  }

  return PDM_OK;
}

/**
 *  @brief Pause recording
  * @param None
 *  @retval 1 if success, 0 otherwise
 */
int pdm_low_level_pause(void)
{
  if (HAL_SAI_DMAPause(&hsai_BlockB1) != HAL_OK) {
    return PDM_ERROR;
  }

  return PDM_OK;
}

/**
 *  @brief Resume recording
  * @param None
 *  @retval 1 if success, 0 otherwise
 */
int pdm_low_level_resume(void)
{
  if (HAL_SAI_DMAResume(&hsai_BlockB1) != HAL_OK) {
    return PDM_ERROR;
  }

  return PDM_OK;
}

/**
 *  @brief Half Callback
 *  @param pBuf: pointer to a buffer to store data
 *  @retval None
 */
void pdm_low_level_copy_half_cplt(uint16_t *pBuf)
{
  uint32_t index;
  uint16_t *DataTempSAI = SAI_InternalBuffer;

  for (index = 0; index < (SAI_InternalBufferSize / 4U); index++) {
    pBuf[index] = (DataTempSAI[index]);
  }
}

/**
 *  @brief Complet Callback
 *  @param pBuf: pointer to a buffer to store data
 *  @retval None
 */
void pdm_low_level_copy_cplt(uint16_t *pBuf)
{
  uint32_t index;
  uint16_t *DataTempSAI = &SAI_InternalBuffer[SAI_InternalBufferSize / 4U];

  for (index = 0; index < (SAI_InternalBufferSize / 4U); index++) {
    pBuf[index] = (DataTempSAI[index]);
  }
}

/* This function is defined weak in the Arduino Core, so we can override it */

/**
 *  @brief System Clock Parameters Configuration
  * @param None
  * @retval None
 */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {};

  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE | RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 40;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
    while (1);
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                                | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK) {
    while (1);
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2 | RCC_PERIPHCLK_SAI1;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  PeriphClkInit.Sai1ClockSelection = RCC_SAI1CLKSOURCE_PLLSAI1;
  PeriphClkInit.PLLSAI1.PLLSAI1Source = RCC_PLLSOURCE_MSI;
  PeriphClkInit.PLLSAI1.PLLSAI1M = 1;
  PeriphClkInit.PLLSAI1.PLLSAI1N = 18;
  PeriphClkInit.PLLSAI1.PLLSAI1P = RCC_PLLP_DIV7;
  PeriphClkInit.PLLSAI1.PLLSAI1Q = RCC_PLLQ_DIV2;
  PeriphClkInit.PLLSAI1.PLLSAI1R = RCC_PLLR_DIV2;
  PeriphClkInit.PLLSAI1.PLLSAI1ClockOut = RCC_PLLSAI1_SAI1CLK;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) {
    while (1);
  }

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK) {
    while (1);
  }
  /** Enable MSI Auto calibration
  */
  HAL_RCCEx_EnableMSIPLLMode();
}

/**
  * @brief SAI1 Initialization Function
  * @param None
  * @retval None
  */
static int MX_SAI1_Init(void)
{
  int ret = PDM_OK;

  __HAL_SAI_RESET_HANDLE_STATE(&hsai_BlockB1);

  hsai_BlockB1.Instance = SAI1_Block_B;
  __HAL_SAI_DISABLE(&hsai_BlockB1);

  hsai_BlockB1.Instance = SAI1_Block_B;
  hsai_BlockB1.Init.Protocol = SAI_FREE_PROTOCOL;
  hsai_BlockB1.Init.AudioMode = SAI_MODEMASTER_RX;
  hsai_BlockB1.Init.DataSize = SAI_DATASIZE_16;
  hsai_BlockB1.Init.FirstBit = SAI_FIRSTBIT_MSB;
  hsai_BlockB1.Init.ClockStrobing = SAI_CLOCKSTROBING_RISINGEDGE;
  hsai_BlockB1.Init.Synchro = SAI_ASYNCHRONOUS;
  hsai_BlockB1.Init.OutputDrive = SAI_OUTPUTDRIVE_DISABLE;
  hsai_BlockB1.Init.NoDivider = SAI_MASTERDIVIDER_ENABLE;
  hsai_BlockB1.Init.FIFOThreshold = SAI_FIFOTHRESHOLD_EMPTY;
  hsai_BlockB1.Init.AudioFrequency = SAI_AUDIO_FREQUENCY_MCKDIV;
  hsai_BlockB1.Init.Mckdiv = 4;
  hsai_BlockB1.Init.SynchroExt = SAI_SYNCEXT_DISABLE;
  hsai_BlockB1.Init.MonoStereoMode = SAI_STEREOMODE;
  hsai_BlockB1.Init.CompandingMode = SAI_NOCOMPANDING;
  hsai_BlockB1.FrameInit.FrameLength = 256;
  hsai_BlockB1.FrameInit.ActiveFrameLength = 128;
  hsai_BlockB1.FrameInit.FSDefinition = SAI_FS_STARTFRAME;
  hsai_BlockB1.FrameInit.FSPolarity = SAI_FS_ACTIVE_LOW;
  hsai_BlockB1.FrameInit.FSOffset = SAI_FS_FIRSTBIT;
  hsai_BlockB1.SlotInit.FirstBitOffset = 0;
  hsai_BlockB1.SlotInit.SlotSize = SAI_SLOTSIZE_DATASIZE;
  hsai_BlockB1.SlotInit.SlotNumber = 16;
  hsai_BlockB1.SlotInit.SlotActive = 0x0000FFFF;
  if (HAL_SAI_Init(&hsai_BlockB1) != HAL_OK) {
    ret = PDM_ERROR;
  }

  /* Enable SAI to generate clock used by audio driver */
  __HAL_SAI_ENABLE(&hsai_BlockB1);

  return ret;
}

/**
* @brief SAI MSP Initialization
* This function configures the hardware resources used in this example
* @param hsai: I2S handle pointer
* @retval None
*/
void HAL_SAI_MspInit(SAI_HandleTypeDef *hsai)
{

  GPIO_InitTypeDef GPIO_InitStruct = {};
  /* SAI1 */
  if (hsai->Instance == SAI1_Block_B) {
    /* Peripheral clock enable */
    if (SAI1_client == 0) {
      __HAL_RCC_SAI1_CLK_ENABLE();
    }
    SAI1_client ++;

    __HAL_RCC_GPIOB_CLK_ENABLE();

    /**SAI1_B_Block_B GPIO Configuration
    PB3 (JTDO-TRACESWO)     ------> SAI1_SCK_B
    PB5     ------> SAI1_SD_B
    */

    GPIO_InitStruct.Pin = GPIO_PIN_3 | GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF13_SAI1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* Peripheral DMA init*/

    hdma_sai1_b.Instance = DMA2_Channel2;
    hdma_sai1_b.Init.Request = DMA_REQUEST_1;
    hdma_sai1_b.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_sai1_b.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_sai1_b.Init.MemInc = DMA_MINC_ENABLE;
    hdma_sai1_b.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hdma_sai1_b.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    hdma_sai1_b.Init.Mode = DMA_CIRCULAR;
    hdma_sai1_b.Init.Priority = DMA_PRIORITY_HIGH;

    if (HAL_DMA_DeInit(&hdma_sai1_b) != HAL_OK) {
      while (1);
    }

    if (HAL_DMA_Init(&hdma_sai1_b) != HAL_OK) {
      while (1);
    }

    /* Several peripheral DMA handle pointers point to the same DMA handle.
       Be aware that there is only one channel to perform all the requested DMAs. */
    __HAL_LINKDMA(hsai, hdmarx, hdma_sai1_b);
    __HAL_LINKDMA(hsai, hdmatx, hdma_sai1_b);
  }
}

/**
* @brief SAI MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param hsai: SAI handle pointer
* @retval None
*/
void HAL_SAI_MspDeInit(SAI_HandleTypeDef *hsai)
{
  /* SAI1 */
  if (hsai->Instance == SAI1_Block_B) {
    SAI1_client --;
    if (SAI1_client == 0) {
      /* Peripheral clock disable */
      __HAL_RCC_SAI1_CLK_DISABLE();
    }

    /**SAI1_B_Block_B GPIO Configuration
    PB3 (JTDO-TRACESWO)     ------> SAI1_SCK_B
    PB5     ------> SAI1_SD_B
    */

    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_3 | GPIO_PIN_5);

    HAL_DMA_DeInit(hsai->hdmarx);
    HAL_DMA_DeInit(hsai->hdmatx);
  }
}


/**
  * @brief Enable DMA controller clock
  * @param None
  * @retval None
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA2_Channel2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Channel2_IRQn, 6, 0);
  HAL_NVIC_EnableIRQ(DMA2_Channel2_IRQn);

}

/**
  * @brief IRQ Handler
  * This function handles DMA2 Channel2 global interrupt.
  * @param None
  * @retval None
  */
void DMA2_Channel2_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_sai1_b);
}

void HAL_SAI_RxHalfCpltCallback(SAI_HandleTypeDef *hSai)
{
  (void)hSai;
  PDM.DMAHalfCpltCallback();
}

void HAL_SAI_RxCpltCallback(SAI_HandleTypeDef *hSai)
{
  (void)hSai;
  PDM.DMACpltCallback();
}

#ifdef __cplusplus
}
#endif

#endif /* ARDUINO_NUCLEO_L476RG */
