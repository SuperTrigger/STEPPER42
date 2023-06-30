#ifndef __STEPPER_H
#define __STEPPER_H

#include "main.h"

/*******************************************************/
#define MOTOR_PUL_IRQn                      TIM8_CC_IRQn
#define MOTOR_PUL_IRQHandler                TIM8_CC_IRQHandler
#define MOTOR_PUL_TIM                       TIM8
#define MOTOR_PUL_CLK_ENABLE()  		    __HAL_RCC_TIM8_CLK_ENABLE()
#define MOTOR_PUL_GPIO_AF                   GPIO_AF3_TIM8

// Motor ����
#define MOTOR_DIR_PIN                  	    GPIO_PIN_0  
#define MOTOR_DIR_GPIO_PORT            	    GPIOG
#define MOTOR_DIR_GPIO_CLK_ENABLE()   	    __HAL_RCC_GPIOG_CLK_ENABLE()

// Motor ʹ��
#define MOTOR_EN_PIN                  	    GPIO_PIN_1
#define MOTOR_EN_GPIO_PORT            	    GPIOG
#define MOTOR_EN_GPIO_CLK_ENABLE()    	    __HAL_RCC_GPIOG_CLK_ENABLE()

// Motor ����
#define MOTOR_PUL_PORT       			    GPIOC
#define MOTOR_PUL_PIN             		    GPIO_PIN_6
#define MOTOR_PUL_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOC_CLK_ENABLE()

// ��ʱ��ͨ��
#define MOTOR_PUL_CHANNEL                   TIM_CHANNEL_1

// ����ʹ������
#define MOTOR_OFFLINE(port, pin, x)       HAL_GPIO_WritePin(port, pin, x)
#define MOTOR_PUL(port, pin, x)           HAL_GPIO_WritePin(port, pin, x)
#define MOTOR_DIR(port, pin, x)           HAL_GPIO_WritePin(port, pin, x)

// Ƶ����ز���
// ������Ҫ��Ƶ�ʿ����Լ�����
#define TIM_PRESCALER                256
// ���嶨ʱ�����ڣ�����Ƚ�ģʽ��������Ϊ0xFFFF
#define TIM_PERIOD                   0xFFFF

#define member_count                 10000 //����ƶ�����

/************************************************************/
#define HIGH GPIO_PIN_SET	    //�ߵ�ƽ
#define LOW  GPIO_PIN_RESET	    //�͵�ƽ

#define ON   HIGH	            //��
#define OFF  LOW	            //��

#define CW   HIGH		        //˳ʱ��
#define CCW  LOW      	        //��ʱ��


void stepper_Init(void);
void stepper_Start(uint32_t channel);
void stepper_Stop(uint32_t channel);

#endif
