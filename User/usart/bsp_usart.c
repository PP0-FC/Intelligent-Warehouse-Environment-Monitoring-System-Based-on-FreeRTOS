#include "bsp_usart.h"

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart3;
DMA_HandleTypeDef hdma_usart3_rx;

/**
  * @brief  ESP8266 RX DMA 初始化
  * @note   配置 DMA1 用于 USART3 接收数据
  * @param  无
  * @retval 无
  */
void ESP8266_RX_DMA_Init(void)
{
    __HAL_RCC_DMA1_CLK_ENABLE();                   // 使能 DMA1 控制器时钟

    HAL_NVIC_SetPriority(DMA1_Channel3_IRQn, 6, 0);// 设置 DMA1_Channel3 中断优先级
    HAL_NVIC_EnableIRQ(DMA1_Channel3_IRQn);       // 使能 DMA1_Channel3 中断
}

void MX_USART1_UART_Init(void)
{
	
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }

}

void MX_USART3_UART_Init(void)
{
	
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(uartHandle->Instance==USART1)
  {

    __HAL_RCC_USART1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
		
		/* USART1 中断配置 */
    HAL_NVIC_SetPriority(USART1_IRQn, 1, 0);          // 设置 USART1 中断优先级为 1
    HAL_NVIC_EnableIRQ(USART1_IRQn);                  // 使能 USART1 中断

  }
	else if(uartHandle->Instance==USART3)
  {

    __HAL_RCC_USART3_CLK_ENABLE();

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**USART3 GPIO Configuration
    PB10     ------> USART1_TX
    PB11     ------> USART1_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
		
		/* USART3 DMA 初始化：RX 使用 DMA1 通道3 */
    hdma_usart3_rx.Instance = DMA1_Channel3;         // 选择 DMA1 通道3
    hdma_usart3_rx.Init.Direction = DMA_PERIPH_TO_MEMORY; // 数据方向：外设到内存
    hdma_usart3_rx.Init.PeriphInc = DMA_PINC_DISABLE;     // 外设地址不自增
    hdma_usart3_rx.Init.MemInc = DMA_MINC_ENABLE;         // 内存地址自增
    hdma_usart3_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE; // 外设数据按字节对齐
    hdma_usart3_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;    // 内存数据按字节对齐
    hdma_usart3_rx.Init.Mode = DMA_NORMAL;            // 普通模式（非循环）
    hdma_usart3_rx.Init.Priority = DMA_PRIORITY_LOW;  // DMA 优先级低
    if (HAL_DMA_Init(&hdma_usart3_rx) != HAL_OK)      // 初始化 DMA 并检查是否成功
    {
      Error_Handler();                                // 初始化失败，进入错误处理
    }

    __HAL_LINKDMA(uartHandle,hdmarx,hdma_usart3_rx);  // 关联 DMA 句柄到 UART 的 hdmarx 成员

    /* USART3 中断配置 */
    HAL_NVIC_SetPriority(USART3_IRQn, 6, 0);          // 设置 USART3 中断优先级为 1
    HAL_NVIC_EnableIRQ(USART3_IRQn);                  // 使能 USART3 中断

  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART1)
  {
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();

    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_10);

  }
	else if(uartHandle->Instance==USART3)
  {
    /* Peripheral clock disable */
    __HAL_RCC_USART3_CLK_DISABLE();

    /**USART3 GPIO Configuration
    PB10     ------> USART1_TX
    PB11     ------> USART1_RX
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_10|GPIO_PIN_11);

  }
}

int fputc(int ch, FILE *f)
{
         HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
         return ch;
}

