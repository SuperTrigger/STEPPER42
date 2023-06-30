#ifndef __STEPPER_H
#define __STEPPER_H

#include "main.h"

/*******************************************************/
#define MOTOR_PUL_IRQn                      TIM8_CC_IRQn
#define MOTOR_PUL_IRQHandler                TIM8_CC_IRQHandler
#define MOTOR_PUL_TIM                       TIM8
#define MOTOR_PUL_CLK_ENABLE()  		    __HAL_RCC_TIM8_CLK_ENABLE()
#define MOTOR_PUL_GPIO_AF                   GPIO_AF3_TIM8

// Motor 方向
#define MOTOR_DIR_PIN                  	    GPIO_PIN_0  
#define MOTOR_DIR_GPIO_PORT            	    GPIOG
#define MOTOR_DIR_GPIO_CLK_ENABLE()   	    __HAL_RCC_GPIOG_CLK_ENABLE()

// Motor 使能
#define MOTOR_EN_PIN                  	    GPIO_PIN_1
#define MOTOR_EN_GPIO_PORT            	    GPIOG
#define MOTOR_EN_GPIO_CLK_ENABLE()    	    __HAL_RCC_GPIOG_CLK_ENABLE()

// Motor 脉冲
#define MOTOR_PUL_PORT       			    GPIOC
#define MOTOR_PUL_PIN             		    GPIO_PIN_6
#define MOTOR_PUL_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOC_CLK_ENABLE()

// 定时器通道
#define MOTOR_PUL_CHANNEL                   TIM_CHANNEL_1

// 控制使能引脚
#define MOTOR_OFFLINE(port, pin, x)       HAL_GPIO_WritePin(port, pin, x)
#define MOTOR_PUL(port, pin, x)           HAL_GPIO_WritePin(port, pin, x)
#define MOTOR_DIR(port, pin, x)           HAL_GPIO_WritePin(port, pin, x)

// 频率相关参数
// 具体需要的频率可以自己计算
#define TIM_PRESCALER                256
// 定义定时器周期，输出比较模式周期设置为0xFFFF
#define TIM_PERIOD                   0xFFFF

#define member_count                 10000 //电机移动距离

/************************************************************/
#define HIGH GPIO_PIN_SET	    //高电平
#define LOW  GPIO_PIN_RESET	    //低电平

#define ON   HIGH	            //开
#define OFF  LOW	            //关

#define CW   HIGH		        //顺时针
#define CCW  LOW      	        //逆时针


void stepper_Init(void);
void stepper_Start(uint32_t channel);
void stepper_Stop(uint32_t channel);

#endif
