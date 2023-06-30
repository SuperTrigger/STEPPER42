#include "stepper.h"

// ��ʱ����� 
TIM_HandleTypeDef TIM_StepperHandle;

// �ж����ȼ�����
static void TIMx_NVIC_Configuration(void)
{
  // �����ж����� 
  HAL_NVIC_SetPriority(MOTOR_PUL_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(MOTOR_PUL_IRQn);
}

// ����TIM�������PWMʱ�õ���I/O
static void Stepper_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    
    MOTOR_DIR_GPIO_CLK_ENABLE();
    MOTOR_EN_GPIO_CLK_ENABLE();
    MOTOR_PUL_GPIO_CLK_ENABLE();
    
    // ѡ��Ҫ���Ƶ�GPIO����															   
    GPIO_InitStruct.Pin = MOTOR_DIR_PIN;	
    // �������ŵ��������Ϊ�������
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;  
    GPIO_InitStruct.Pull =GPIO_PULLUP;
    // ������������Ϊ����  
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    // Motor �������� ��ʼ��
    HAL_GPIO_Init(MOTOR_DIR_GPIO_PORT, &GPIO_InitStruct);
    MOTOR_DIR(MOTOR_DIR_GPIO_PORT, MOTOR_DIR_PIN, CW);
    
    // Motor ʹ������ ��ʼ��
    GPIO_InitStruct.Pin = MOTOR_EN_PIN;
    HAL_GPIO_Init(MOTOR_EN_GPIO_PORT, &GPIO_InitStruct);
    MOTOR_OFFLINE(MOTOR_EN_GPIO_PORT, MOTOR_EN_PIN, ON); 
    
    // ��ʱ�����ͨ����������IO��ʼ�� 
    // �����������
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    // ������������ 
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    // ���ø���
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    // ѡ��Ҫ���Ƶ�GPIO����
    GPIO_InitStruct.Pin = MOTOR_PUL_PIN;
    // Motor �������� ��ʼ��
    HAL_GPIO_Init(MOTOR_PUL_PORT, &GPIO_InitStruct);
    
}

static void TIM_PWMOUTPUT_Config(void)
{
	TIM_OC_InitTypeDef  TIM_OCInitStructure;

	// ʹ�ܶ�ʱ��
	MOTOR_PUL_CLK_ENABLE();

	TIM_StepperHandle.Instance = MOTOR_PUL_TIM;    
	TIM_StepperHandle.Init.Period = TIM_PERIOD;
	TIM_StepperHandle.Init.Prescaler = TIM_PRESCALER - 1;                
	// ������ʽ
	TIM_StepperHandle.Init.CounterMode = TIM_COUNTERMODE_UP;            
	// ����ʱ�ӷ�Ƶ
	TIM_StepperHandle.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;   
	TIM_StepperHandle.Init.RepetitionCounter = 0 ;  		
	// ��ʼ����ʱ��
	HAL_TIM_OC_Init(&TIM_StepperHandle);

	// PWMģʽ����--��������Ϊ����Ƚ�ģʽ
	TIM_OCInitStructure.OCMode = TIM_OCMODE_TOGGLE; 
	// �Ƚ�����ļ���ֵ
	TIM_OCInitStructure.Pulse = TIM_PERIOD;                    
	// ����ʱ������ֵС��CCR1_ValʱΪ�ߵ�ƽ
	TIM_OCInitStructure.OCPolarity = TIM_OCPOLARITY_HIGH;          
	// ���û���ͨ������ļ���
	TIM_OCInitStructure.OCNPolarity = TIM_OCNPOLARITY_LOW; 
	// ����ģʽ����
	TIM_OCInitStructure.OCFastMode = TIM_OCFAST_DISABLE;   
	// ���е�ƽ
	TIM_OCInitStructure.OCIdleState = TIM_OCIDLESTATE_RESET;  
	// ����ͨ������
	TIM_OCInitStructure.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  
    // �����Ƚ������ʹ���ж� 
    HAL_TIM_OC_ConfigChannel(&TIM_StepperHandle, &TIM_OCInitStructure, MOTOR_PUL_CHANNEL);
    HAL_TIM_OC_Start_IT(&TIM_StepperHandle, MOTOR_PUL_CHANNEL);
  
    // ������ʱ�� 
	HAL_TIM_Base_Start(&TIM_StepperHandle);
}

// ��ʱ���жϷ�����
void MOTOR_PUL_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&TIM_StepperHandle);
}

// ���������ʼ��
void stepper_Init(void)
{
	// ���IO����
	Stepper_GPIO_Config();
	// ��ʱ��PWM�������
	TIM_PWMOUTPUT_Config();
	// �ж�����
	TIMx_NVIC_Configuration();
}

// ��ʱ���Ƚ��жϻص�����
void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim)
{
    static uint32_t step = 0;
    if (step < member_count)
    {
        // ��ȡ��ǰ���� 
        uint32_t count = __HAL_TIM_GET_COUNTER(htim);
        uint16_t oc_pulse_num = 100; //����Ƚϼ���ֵ��ֵԽС���ת��Խ��
        __HAL_TIM_SET_COMPARE(&TIM_StepperHandle, MOTOR_PUL_CHANNEL, count + oc_pulse_num);
        step++;
    }
    if (step == member_count * SPR)
    {
        stepper_Stop(MOTOR_PUL_CHANNEL);
        step = 0;
    }
}

// �������
void stepper_Start(uint32_t channel)
{
    TIM_CCxChannelCmd(MOTOR_PUL_TIM, channel, TIM_CCx_ENABLE);
}

// ֹͣ���
void stepper_Stop(uint32_t channel)
{
    TIM_CCxChannelCmd(MOTOR_PUL_TIM, channel, TIM_CCx_DISABLE);
}
