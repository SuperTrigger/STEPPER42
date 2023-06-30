#include "stepper.h"

// 定时器句柄 
TIM_HandleTypeDef TIM_StepperHandle;

// 中断优先级配置
static void TIMx_NVIC_Configuration(void)
{
  // 外设中断配置 
  HAL_NVIC_SetPriority(MOTOR_PUL_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(MOTOR_PUL_IRQn);
}

// 配置TIM复用输出PWM时用到的I/O
static void Stepper_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    
    MOTOR_DIR_GPIO_CLK_ENABLE();
    MOTOR_EN_GPIO_CLK_ENABLE();
    MOTOR_PUL_GPIO_CLK_ENABLE();
    
    // 选择要控制的GPIO引脚															   
    GPIO_InitStruct.Pin = MOTOR_DIR_PIN;	
    // 设置引脚的输出类型为推挽输出
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;  
    GPIO_InitStruct.Pull =GPIO_PULLUP;
    // 设置引脚速率为高速  
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    // Motor 方向引脚 初始化
    HAL_GPIO_Init(MOTOR_DIR_GPIO_PORT, &GPIO_InitStruct);
    MOTOR_DIR(MOTOR_DIR_GPIO_PORT, MOTOR_DIR_PIN, CW);
    
    // Motor 使能引脚 初始化
    GPIO_InitStruct.Pin = MOTOR_EN_PIN;
    HAL_GPIO_Init(MOTOR_EN_GPIO_PORT, &GPIO_InitStruct);
    MOTOR_OFFLINE(MOTOR_EN_GPIO_PORT, MOTOR_EN_PIN, ON); 
    
    // 定时器输出通道功能引脚IO初始化 
    // 设置输出类型
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    // 设置引脚速率 
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    // 设置复用
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    // 选择要控制的GPIO引脚
    GPIO_InitStruct.Pin = MOTOR_PUL_PIN;
    // Motor 脉冲引脚 初始化
    HAL_GPIO_Init(MOTOR_PUL_PORT, &GPIO_InitStruct);
    
}

static void TIM_PWMOUTPUT_Config(void)
{
	TIM_OC_InitTypeDef  TIM_OCInitStructure;

	// 使能定时器
	MOTOR_PUL_CLK_ENABLE();

	TIM_StepperHandle.Instance = MOTOR_PUL_TIM;    
	TIM_StepperHandle.Init.Period = TIM_PERIOD;
	TIM_StepperHandle.Init.Prescaler = TIM_PRESCALER - 1;                
	// 计数方式
	TIM_StepperHandle.Init.CounterMode = TIM_COUNTERMODE_UP;            
	// 采样时钟分频
	TIM_StepperHandle.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;   
	TIM_StepperHandle.Init.RepetitionCounter = 0 ;  		
	// 初始化定时器
	HAL_TIM_OC_Init(&TIM_StepperHandle);

	// PWM模式配置--这里配置为输出比较模式
	TIM_OCInitStructure.OCMode = TIM_OCMODE_TOGGLE; 
	// 比较输出的计数值
	TIM_OCInitStructure.Pulse = TIM_PERIOD;                    
	// 当定时器计数值小于CCR1_Val时为高电平
	TIM_OCInitStructure.OCPolarity = TIM_OCPOLARITY_HIGH;          
	// 设置互补通道输出的极性
	TIM_OCInitStructure.OCNPolarity = TIM_OCNPOLARITY_LOW; 
	// 快速模式设置
	TIM_OCInitStructure.OCFastMode = TIM_OCFAST_DISABLE;   
	// 空闲电平
	TIM_OCInitStructure.OCIdleState = TIM_OCIDLESTATE_RESET;  
	// 互补通道设置
	TIM_OCInitStructure.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  
    // 启动比较输出并使能中断 
    HAL_TIM_OC_ConfigChannel(&TIM_StepperHandle, &TIM_OCInitStructure, MOTOR_PUL_CHANNEL);
    HAL_TIM_OC_Start_IT(&TIM_StepperHandle, MOTOR_PUL_CHANNEL);
  
    // 启动定时器 
	HAL_TIM_Base_Start(&TIM_StepperHandle);
}

// 定时器中断服务函数
void MOTOR_PUL_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&TIM_StepperHandle);
}

// 步进电机初始化
void stepper_Init(void)
{
	// 电机IO配置
	Stepper_GPIO_Config();
	// 定时器PWM输出配置
	TIM_PWMOUTPUT_Config();
	// 中断配置
	TIMx_NVIC_Configuration();
}

// 定时器比较中断回调函数
void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim)
{
    static uint32_t step = 0;
    if (step < member_count)
    {
        // 获取当前计数 
        uint32_t count = __HAL_TIM_GET_COUNTER(htim);
        uint16_t oc_pulse_num = 100; //输出比较计数值，值越小电机转速越快
        __HAL_TIM_SET_COMPARE(&TIM_StepperHandle, MOTOR_PUL_CHANNEL, count + oc_pulse_num);
        step++;
    }
    if (step == member_count * SPR)
    {
        stepper_Stop(MOTOR_PUL_CHANNEL);
        step = 0;
    }
}

// 启动电机
void stepper_Start(uint32_t channel)
{
    TIM_CCxChannelCmd(MOTOR_PUL_TIM, channel, TIM_CCx_ENABLE);
}

// 停止电机
void stepper_Stop(uint32_t channel)
{
    TIM_CCxChannelCmd(MOTOR_PUL_TIM, channel, TIM_CCx_DISABLE);
}
