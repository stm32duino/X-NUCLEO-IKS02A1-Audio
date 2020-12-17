#if defined(ARDUINO_NUCLEO_F401RE)

#include "PDM.h"


#ifdef __cplusplus
extern "C" {
#endif

I2S_HandleTypeDef hi2s3;
DMA_HandleTypeDef hdma_spi3_rx;

uint16_t I2S_InternalBuffer[((AUDIO_IN_FREQ / 8) * MAX_AUDIO_IN_CHANNEL_NBR_PER_IF * N_MS_PER_INTERRUPT)];
uint32_t I2S_InternalBufferSize = (((AUDIO_IN_FREQ / 8) * MAX_AUDIO_IN_CHANNEL_NBR_PER_IF *N_MS_PER_INTERRUPT) * 2);

static void MX_DMA_Init(void);
static int MX_I2S3_Init(void);

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
  ret = MX_I2S3_Init();
  return ret;
}

/**
 *  @brief PDM deinitialization
 *  @param None
 *  @retval 1 if success, 0 otherwise
 */
int pdm_low_level_deinit(void)
{
  if (HAL_I2S_DeInit(&hi2s3) != HAL_OK) {
    return PDM_ERROR;
  }

  return PDM_OK;
}

/**
 *  @brief Start recording
 *  @param None
 *  @retval 1 if success, 0 otherwise
 */
int pdm_low_level_record(void)
{
  if (HAL_I2S_Receive_DMA(&hi2s3, I2S_InternalBuffer, (uint16_t)I2S_InternalBufferSize / 2U) != HAL_OK) {
    return PDM_ERROR;
  }

  return PDM_OK;
}

/**
 *  @brief Stop recording
 *  @param None
 *  @retval 1 if success, 0 otherwise
 */
int pdm_low_level_stop(void)
{
  if (HAL_I2S_DMAStop(&hi2s3) != HAL_OK) {
    return PDM_ERROR;
  }

  return PDM_OK;
}

/**
 *  @brief Pause recording
 *  @param None
 *  @retval 1 if success, 0 otherwise
 */
int pdm_low_level_pause(void)
{
  if (HAL_I2S_DMAPause(&hi2s3) != HAL_OK) {
    return PDM_ERROR;
  }

  return PDM_OK;
}

/**
 *  @brief Resume recording
 *  @param None
 *  @retval 1 if success, 0 otherwise
 */
int pdm_low_level_resume(void)
{
  if (HAL_I2S_DMAResume(&hi2s3) != HAL_OK) {
    return PDM_ERROR;
  }

  return PDM_OK;
}

/**
 *  @brief Half Callback
 *  @param pBuf pointer to a buffer to store data
 *  @retval None
 */
void pdm_low_level_copy_half_cplt(uint16_t *pBuf)
{
  uint32_t index;
  uint16_t *DataTempI2S = I2S_InternalBuffer;

  for (index = 0; index < (I2S_InternalBufferSize / 4U); index++) {
    pBuf[index] = (DataTempI2S[index]);
  }
}

/**
 *  @brief Complet Callback
 *  @param pBuf pointer to a buffer to store data
 *  @retval None
 */
void pdm_low_level_copy_cplt(uint16_t *pBuf)
{
  uint32_t index;
  uint16_t *DataTempI2S = &I2S_InternalBuffer[I2S_InternalBufferSize / 4U];

  for (index = 0; index < (I2S_InternalBufferSize / 4U); index++) {
    pBuf[index] = (DataTempI2S[index]);
  }
}


/* This function is defined weak in the Arduino Core, so we can override it */

/**
 *  @brief System Clock Parameters Configuration
 *  @param None
 *  @retval None
 */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
    while (1);
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                                | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
    while (1);
  }
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_I2S;
  PeriphClkInitStruct.PLLI2S.PLLI2SN = 192;
  PeriphClkInitStruct.PLLI2S.PLLI2SR = 2;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK) {
    while (1);
  }
}

/**
  * @brief I2S3 Initialization Function
  * @param None
  * @retval None
  */
static int MX_I2S3_Init(void)
{
  int ret = PDM_OK;

  hi2s3.Instance = SPI3;

  __HAL_I2S_DISABLE(&hi2s3);

  hi2s3.Instance = SPI3;
  hi2s3.Init.Mode = I2S_MODE_MASTER_RX;
  hi2s3.Init.Standard = I2S_STANDARD_MSB;
  hi2s3.Init.DataFormat = I2S_DATAFORMAT_16B;
  hi2s3.Init.MCLKOutput = I2S_MCLKOUTPUT_DISABLE;
  hi2s3.Init.AudioFreq = 40000;
  hi2s3.Init.CPOL = I2S_CPOL_HIGH;
  hi2s3.Init.ClockSource = I2S_CLOCK_PLL;
  hi2s3.Init.FullDuplexMode = I2S_FULLDUPLEXMODE_DISABLE;
  if (HAL_I2S_Init(&hi2s3) != HAL_OK) {
    ret = PDM_ERROR;
  }

  return ret;
}

/**
* @brief I2S MSP Initialization
* This function configures the hardware resources used in this example
* @param hi2s: I2S handle pointer
* @retval None
*/
void HAL_I2S_MspInit(I2S_HandleTypeDef *hi2s)
{
  GPIO_InitTypeDef GPIO_InitStruct = {};
  if (hi2s->Instance == SPI3) {
    /* Peripheral clock enable */
    __HAL_RCC_SPI3_CLK_ENABLE();

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**I2S3 GPIO Configuration
    PB3     ------> I2S3_CK
    PB5     ------> I2S3_SD
    */

    GPIO_InitStruct.Pin = GPIO_PIN_3 | GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
    GPIO_InitStruct.Alternate = GPIO_AF6_SPI3;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* I2S3 DMA Init */
    /* SPI3_RX Init */
    hdma_spi3_rx.Instance = DMA1_Stream0;
    hdma_spi3_rx.Init.Channel = DMA_CHANNEL_0;
    hdma_spi3_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_spi3_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_spi3_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_spi3_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hdma_spi3_rx.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    hdma_spi3_rx.Init.Mode = DMA_CIRCULAR;
    hdma_spi3_rx.Init.Priority = DMA_PRIORITY_HIGH;
    hdma_spi3_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    hdma_spi3_rx.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
    hdma_spi3_rx.Init.MemBurst = DMA_MBURST_SINGLE;
    hdma_spi3_rx.Init.PeriphBurst = DMA_MBURST_SINGLE;

    if (HAL_DMA_DeInit(&hdma_spi3_rx) != HAL_OK) {
      while (1);
    }

    if (HAL_DMA_Init(&hdma_spi3_rx) != HAL_OK) {
      while (1);
    }

    __HAL_LINKDMA(hi2s, hdmarx, hdma_spi3_rx);
  }
}

/**
* @brief I2S MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param hi2s: I2S handle pointer
* @retval None
*/
void HAL_I2S_MspDeInit(I2S_HandleTypeDef *hi2s)
{
  if (hi2s->Instance == SPI3) {
    /* Peripheral clock disable */
    __HAL_RCC_SPI3_CLK_DISABLE();

    /**I2S3 GPIO Configuration
    PB3     ------> I2S3_CK
    PB5     ------> I2S3_SD
    */

    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_3 | GPIO_PIN_5);

    /* I2S3 DMA DeInit */
    HAL_DMA_DeInit(hi2s->hdmarx);
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
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream0_IRQn, 6, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream0_IRQn);

}

/**
  * @brief IRQ Handler
  * This function handles DMA1 stream0 global interrupt.
  * @param None
  * @retval None
  */
void DMA1_Stream0_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_spi3_rx);
}

void HAL_I2S_RxHalfCpltCallback(I2S_HandleTypeDef *hI2S)
{
  (void)hI2S;
  PDM.DMAHalfCpltCallback();
}

void HAL_I2S_RxCpltCallback(I2S_HandleTypeDef *hI2S)
{
  (void)hI2S;
  PDM.DMACpltCallback();
}


#ifdef __cplusplus
}
#endif

#endif /* ARDUINO_NUCLEO_F401RE */
