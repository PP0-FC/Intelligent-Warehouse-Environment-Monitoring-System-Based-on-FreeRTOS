#include "bsp_dht11.h"
#include "bsp_dwt.h"

/**
 * @brief  DHT11 DATA 引脚 IO 初始化
 * @retval 无
 */
void DHT11_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* 使能GPIOB端口时钟 */
    __HAL_RCC_GPIOB_CLK_ENABLE();

    /* 设置引脚为推挽输出 */
    GPIO_InitStruct.Pin = DHT11_DATA_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;     // 推挽输出
    GPIO_InitStruct.Pull = GPIO_NOPULL;             // 不上拉不下拉
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;    // 低速输出
    HAL_GPIO_Init(DHT11_DATA_GPIO_Port, &GPIO_InitStruct);

    /* 初始化DHT11数据引脚 */
    HAL_GPIO_WritePin(DHT11_DATA_GPIO_Port, DHT11_DATA_Pin, GPIO_PIN_SET);
}


/**
 * @brief  配置 DHT11 数据引脚的工作模式
 * @param  mode 引脚模式，如输入、输出等，使用 HAL 库中的 GPIO_MODE_XXX 宏
 * @param  pull 上下拉配置，使用 HAL 库中的 GPIO_PULL_XXX 宏
 */
void DHT11_SetGPIOMode(uint32_t mode, uint32_t pull)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = DHT11_DATA_Pin;        	// 选择DHT11数据引脚
    GPIO_InitStruct.Mode = mode;                  // 设置引脚模式（输入、输出等）
    GPIO_InitStruct.Pull = pull;                  // 设置上拉或下拉
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW; 	// 设置引脚速度

    HAL_GPIO_Init(DHT11_DATA_GPIO_Port, &GPIO_InitStruct);  // 初始化GPIO
}



/**
 * @brief  读取DHT11一个字节数据
 * @retval 返回8位数据（1字节）
 */
uint8_t DHT11_ReadByte(void)
{
    uint8_t value = 0;

    for (uint8_t i = 0; i < 8; i++)
    {
        // 等待信号线变高，表示开始传输第i位数据
        while (HAL_GPIO_ReadPin(DHT11_DATA_GPIO_Port, DHT11_DATA_Pin) == GPIO_PIN_RESET);

        DWT_DelayUs(40);  // 延时40微秒，判断数据位是0还是1

        // 如果40us后线仍然为高，表示该位为1
        if (HAL_GPIO_ReadPin(DHT11_DATA_GPIO_Port, DHT11_DATA_Pin) == GPIO_PIN_SET)
        {
            value |= (1 << (7 - i));  // 把对应位设置为1

            // 等待信号线变低，准备接收下一位
            while (HAL_GPIO_ReadPin(DHT11_DATA_GPIO_Port, DHT11_DATA_Pin) == GPIO_PIN_SET);
        }
        // 如果40us后线为低，则该位为0，直接继续下一位
    }

    return value;
}


/**
 * @brief  读取DHT11传感器数据
 * @param  data 指向存放温湿度数据的结构体指针
 * @retval HAL_OK 成功，HAL_ERROR 失败
 */
HAL_StatusTypeDef DHT11_ReadData(DHT11_DATA_TYPEDEF *data)
{
    uint8_t retry = 0;

    // 1. 主机拉低总线，发送起始信号（至少18ms）
    DHT11_SetGPIOMode(GPIO_MODE_OUTPUT_PP, GPIO_NOPULL);     // 设置为推挽输出
    HAL_GPIO_WritePin(DHT11_DATA_GPIO_Port, DHT11_DATA_Pin, GPIO_PIN_RESET);
    DWT_DelayMs(20);  // 保持低电平20ms，通知DHT11开始传输
    HAL_GPIO_WritePin(DHT11_DATA_GPIO_Port, DHT11_DATA_Pin, GPIO_PIN_SET);
    DWT_DelayUs(30); 	// 延时30微秒

    // 2. 设置引脚为输入，等待DHT11响应信号
    DHT11_SetGPIOMode(GPIO_MODE_INPUT, GPIO_PULLUP);

    // 3. 等待DHT11拉低响应信号（最大等待100us）
    retry = 0;
    while (HAL_GPIO_ReadPin(DHT11_DATA_GPIO_Port, DHT11_DATA_Pin) == GPIO_PIN_SET)
    {
        if (++retry > 100)
            return HAL_ERROR;    // 超时无响应，读取失败
        DWT_DelayUs(1);
    }

    // 等待DHT11拉高信号（最大等待100us）
    retry = 0;
    while (HAL_GPIO_ReadPin(DHT11_DATA_GPIO_Port, DHT11_DATA_Pin) == GPIO_PIN_RESET)
    {
        if (++retry > 100)
            return HAL_ERROR;    // 超时，读取失败
        DWT_DelayUs(1);
    }

    // 等待DHT11再次拉低信号（最大等待100us）
    retry = 0;
    while (HAL_GPIO_ReadPin(DHT11_DATA_GPIO_Port, DHT11_DATA_Pin) == GPIO_PIN_SET)
    {
        if (++retry > 100)
            return HAL_ERROR;    // 超时，读取失败
        DWT_DelayUs(1);
    }

    // 4. 读取5字节数据（湿度整数、小数，温度整数、小数，校验和）
    data->humi_int   = DHT11_ReadByte();
    data->humi_deci  = DHT11_ReadByte();
    data->temp_int   = DHT11_ReadByte();
    data->temp_deci  = DHT11_ReadByte();
    data->check_sum  = DHT11_ReadByte();

    // 5. 校验数据是否正确
    uint8_t sum = data->humi_int + data->humi_deci + data->temp_int + data->temp_deci;
    return (sum == data->check_sum) ? HAL_OK : HAL_ERROR;
}
