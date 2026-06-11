#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_exti.h"
#include "misc.h"
#include <stdio.h>

/* Global Variables */
uint8_t car_mode = 0;           /* 0:蓝牙模式, 1:循迹模式, 2:避障模式 */
uint8_t car_speed = 1;          /* 0:停止, 1:低速, 2:中速, 3:高速 */
uint8_t car_direction = 0;      /* 0:停止, 1:前进, 2:后退, 3:左转, 4:右转 */
uint16_t sonic_distance = 0;    /* 超声波距离(cm) */
uint8_t trace_status = 0;       /* 循迹状态 */

/* Function Prototypes */
void RCC_Configuration(void);
void GPIO_Configuration(void);
void TIM_Configuration(void);
void USART_Configuration(void);
void EXTI_Configuration(void);
void NVIC_Configuration(void);
void Delay_ms(uint32_t ms);
void Delay_us(uint32_t us);
void Motor_Init(void);
void Motor_Control(uint8_t direction, uint8_t speed);
void Sonic_Trigger(void);
uint16_t Sonic_GetDistance(void);
void LCD_Init(void);
void LCD_DisplayInfo(void);
void USART_SendChar(uint8_t ch);
uint8_t USART_ReceiveChar(void);
void USART_SendString(const char *str);
void Bluetooth_Control(uint8_t cmd);

/**
 * @brief  Main function
 * @retval None
 */
int main(void)
{
    /* System Initialization */
    RCC_Configuration();
    GPIO_Configuration();
    TIM_Configuration();
    USART_Configuration();
    EXTI_Configuration();
    NVIC_Configuration();
    
    /* Device Initialization */
    Motor_Init();
    LCD_Init();
    
    Delay_ms(100);
    
    /* Main Loop */
    while(1)
    {
        if(car_mode == 0)   /* 蓝牙控制模式 */
        {
            if(USART_GetFlagStatus(USART2, USART_FLAG_RXNE) != RESET)
            {
                uint8_t cmd = USART_ReceiveData(USART2);
                Bluetooth_Control(cmd);
            }
        }
        else if(car_mode == 1)  /* 自动循迹模式 */
        {
            trace_status = (GPIOB->IDR >> 0) & 0x1F;  /* 读取PB0~PB4 */
            
            if(trace_status == 0x1F)        /* 全部检测到黑线 */
            {
                Motor_Control(1, car_speed); /* 前进 */
            }
            else if((trace_status & 0x08) == 0)  /* 中间检测到 */
            {
                Motor_Control(1, car_speed);
            }
            else if((trace_status & 0x04) == 0)  /* 偏右 */
            {
                Motor_Control(3, car_speed);
            }
            else if((trace_status & 0x10) == 0)  /* 偏左 */
            {
                Motor_Control(4, car_speed);
            }
        }
        else if(car_mode == 2)  /* 自动避障模式 */
        {
            Sonic_Trigger();
            Delay_us(100);
            sonic_distance = Sonic_GetDistance();
            
            if(sonic_distance > 20)
            {
                Motor_Control(1, car_speed); /* 前进 */
            }
            else
            {
                Motor_Control(2, car_speed); /* 后退 */
                Delay_ms(500);
                Motor_Control(4, car_speed); /* 左转 */
                Delay_ms(400);
            }
        }
        
        LCD_DisplayInfo();
        Delay_ms(50);
    }
}

/**
 * @brief  RCC Configuration
 * @retval None
 */
void RCC_Configuration(void)
{
    RCC_DeInit();
    RCC_HSEConfig(RCC_HSE_ON);
    while(RCC_WaitForHSEStartUp() == ERROR);
    
    RCC_PLL_DIV2Config();
    RCC_PLL_Config(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
    RCC_PLLCmd(ENABLE);
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);
    
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
    while(RCC_GetSYSCLKSource() != 0x08);
    
    /* Enable GPIO Clocks */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | 
                           RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);
    
    /* Enable TIM Clocks */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3 | RCC_APB1Periph_TIM4, ENABLE);
    
    /* Enable USART Clocks */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
}

/**
 * @brief  GPIO Configuration
 * @retval None
 */
void GPIO_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    /* Motor Control GPIO: PA0, PA1, PA8, PB15 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    /* PWM for Motor: PA6, PA7 (TIM3_CH1, TIM3_CH2) */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    /* Trace Sensor GPIO: PB0~PB4 (Input) */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    /* HC-SR04 GPIO: PB5(TRIG), PB6(ECHO) */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    /* LCD1602 GPIO: PB8~PB14 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | 
                                  GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    /* Status LED: PC13 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    
    /* USART2 GPIO: PA2(TX), PA3(RX) */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    /* Clear all GPIO outputs */
    GPIO_ResetBits(GPIOA, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_8);
    GPIO_ResetBits(GPIOB, GPIO_Pin_15);
    GPIO_ResetBits(GPIOC, GPIO_Pin_13);
}

/**
 * @brief  TIM Configuration for PWM
 * @retval None
 */
void TIM_Configuration(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;
    
    /* TIM3 for Motor PWM */
    TIM_TimeBaseStructure.TIM_Period = 20000 - 1;      /* 10kHz PWM */
    TIM_TimeBaseStructure.TIM_Prescaler = 3;           /* 72MHz / 4 = 18MHz */
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
    
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OCInitStructure.TIM_Pulse = 0;
    
    TIM_OC1Init(TIM3, &TIM_OCInitStructure);  /* PA6 - ENA */
    TIM_OC2Init(TIM3, &TIM_OCInitStructure);  /* PA7 - ENB */
    
    TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);
    TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);
    
    TIM_ARRPreloadConfig(TIM3, ENABLE);
    TIM_Cmd(TIM3, ENABLE);
}

/**
 * @brief  USART Configuration
 * @retval None
 */
void USART_Configuration(void)
{
    USART_InitTypeDef USART_InitStructure;
    
    /* USART2 Configuration: 9600 8N1 */
    USART_InitStructure.USART_BaudRate = 9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    
    USART_Init(USART2, &USART_InitStructure);
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
    USART_Cmd(USART2, ENABLE);
}

/**
 * @brief  EXTI Configuration
 * @retval None
 */
void EXTI_Configuration(void)
{
    /* Not used in this basic example */
}

/**
 * @brief  NVIC Configuration
 * @retval None
 */
void NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    
    /* Enable USART2 IRQ */
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/**
 * @brief  Delay function (milliseconds)
 * @param  ms: milliseconds
 * @retval None
 */
void Delay_ms(uint32_t ms)
{
    uint32_t i, j;
    for(i = 0; i < ms; i++)
        for(j = 0; j < 123; j++);
}

/**
 * @brief  Delay function (microseconds)
 * @param  us: microseconds
 * @retval None
 */
void Delay_us(uint32_t us)
{
    uint32_t i;
    for(i = 0; i < us * 8; i++);
}

/**
 * @brief  Motor Initialization
 * @retval None
 */
void Motor_Init(void)
{
    TIM_SetCompare1(TIM3, 0);
    TIM_SetCompare2(TIM3, 0);
    GPIO_ResetBits(GPIOA, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_8);
    GPIO_ResetBits(GPIOB, GPIO_Pin_15);
}

/**
 * @brief  Motor Control
 * @param  direction: 0=Stop, 1=Forward, 2=Backward, 3=Left, 4=Right
 * @param  speed: 0=Stop, 1=Low, 2=Medium, 3=High
 * @retval None
 */
void Motor_Control(uint8_t direction, uint8_t speed)
{
    uint16_t pwm_value = 0;
    
    /* Convert speed to PWM value */
    switch(speed)
    {
        case 0: pwm_value = 0; break;
        case 1: pwm_value = 5000; break;   /* 25% */
        case 2: pwm_value = 10000; break;  /* 50% */
        case 3: pwm_value = 19000; break;  /* 95% */
        default: pwm_value = 0;
    }
    
    TIM_SetCompare1(TIM3, pwm_value);  /* ENA */
    TIM_SetCompare2(TIM3, pwm_value);  /* ENB */
    
    switch(direction)
    {
        case 0: /* Stop */
            GPIO_ResetBits(GPIOA, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_8);
            GPIO_ResetBits(GPIOB, GPIO_Pin_15);
            TIM_SetCompare1(TIM3, 0);
            TIM_SetCompare2(TIM3, 0);
            break;
            
        case 1: /* Forward */
            GPIO_SetBits(GPIOA, GPIO_Pin_0);     /* IN1 */
            GPIO_ResetBits(GPIOA, GPIO_Pin_1);   /* IN2 */
            GPIO_SetBits(GPIOA, GPIO_Pin_8);     /* IN3 */
            GPIO_ResetBits(GPIOB, GPIO_Pin_15);  /* IN4 */
            break;
            
        case 2: /* Backward */
            GPIO_ResetBits(GPIOA, GPIO_Pin_0);   /* IN1 */
            GPIO_SetBits(GPIOA, GPIO_Pin_1);     /* IN2 */
            GPIO_ResetBits(GPIOA, GPIO_Pin_8);   /* IN3 */
            GPIO_SetBits(GPIOB, GPIO_Pin_15);    /* IN4 */
            break;
            
        case 3: /* Left Turn */
            GPIO_SetBits(GPIOA, GPIO_Pin_0);     /* IN1 */
            GPIO_ResetBits(GPIOA, GPIO_Pin_1);   /* IN2 */
            GPIO_ResetBits(GPIOA, GPIO_Pin_8);   /* IN3 */
            GPIO_SetBits(GPIOB, GPIO_Pin_15);    /* IN4 */
            break;
            
        case 4: /* Right Turn */
            GPIO_ResetBits(GPIOA, GPIO_Pin_0);   /* IN1 */
            GPIO_SetBits(GPIOA, GPIO_Pin_1);     /* IN2 */
            GPIO_SetBits(GPIOA, GPIO_Pin_8);     /* IN3 */
            GPIO_ResetBits(GPIOB, GPIO_Pin_15);  /* IN4 */
            break;
            
        default:
            Motor_Control(0, 0);
    }
}

/**
 * @brief  HC-SR04 Trigger
 * @retval None
 */
void Sonic_Trigger(void)
{
    GPIO_SetBits(GPIOB, GPIO_Pin_5);    /* TRIG = 1 */
    Delay_us(15);
    GPIO_ResetBits(GPIOB, GPIO_Pin_5);  /* TRIG = 0 */
}

/**
 * @brief  HC-SR04 Get Distance
 * @retval Distance in cm
 */
uint16_t Sonic_GetDistance(void)
{
    uint32_t timer = 0;
    uint16_t distance = 0;
    
    /* Wait for ECHO to go HIGH */
    while((GPIOB->IDR & GPIO_Pin_6) == 0 && timer < 50000)
        timer++;
    
    timer = 0;
    
    /* Measure time while ECHO is HIGH */
    while((GPIOB->IDR & GPIO_Pin_6) != 0 && timer < 50000)
        timer++;
    
    /* Distance = timer * 0.017 cm/us (speed of sound) */
    /* Simplified: distance = timer / 58 */
    distance = timer / 58;
    
    return distance;
}

/**
 * @brief  LCD1602 Initialization
 * @retval None
 */
void LCD_Init(void)
{
    /* Simple initialization sequence for 4-bit mode */
    Delay_ms(50);
    
    /* RS=0, RW=0, E pulse */
    GPIO_ResetBits(GPIOB, GPIO_Pin_12 | GPIO_Pin_13);  /* RS=0, RW=0 */
    GPIO_SetBits(GPIOB, GPIO_Pin_14);                   /* E=1 */
    Delay_ms(1);
    GPIO_ResetBits(GPIOB, GPIO_Pin_14);                 /* E=0 */
}

/**
 * @brief  LCD Display Information
 * @retval None
 */
void LCD_DisplayInfo(void)
{
    /* This is a simplified function. In real implementation,
       you would send proper LCD commands to display mode, speed, distance */
    
    if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13) == 0)
    {
        GPIO_SetBits(GPIOC, GPIO_Pin_13);
    }
    else
    {
        GPIO_ResetBits(GPIOC, GPIO_Pin_13);
    }
}

/**
 * @brief  USART Send Character
 * @param  ch: character to send
 * @retval None
 */
void USART_SendChar(uint8_t ch)
{
    USART_SendData(USART2, ch);
    while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
}

/**
 * @brief  USART Receive Character
 * @retval Received character
 */
uint8_t USART_ReceiveChar(void)
{
    while(USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == RESET);
    return (uint8_t)USART_ReceiveData(USART2);
}

/**
 * @brief  USART Send String
 * @param  str: string to send
 * @retval None
 */
void USART_SendString(const char *str)
{
    while(*str)
    {
        USART_SendChar(*str++);
    }
}

/**
 * @brief  Bluetooth Control
 * @param  cmd: command character
 * @retval None
 */
void Bluetooth_Control(uint8_t cmd)
{
    switch(cmd)
    {
        case 'F': /* Forward */
            Motor_Control(1, car_speed);
            car_direction = 1;
            break;
            
        case 'B': /* Backward */
            Motor_Control(2, car_speed);
            car_direction = 2;
            break;
            
        case 'L': /* Left Turn */
            Motor_Control(3, car_speed);
            car_direction = 3;
            break;
            
        case 'R': /* Right Turn */
            Motor_Control(4, car_speed);
            car_direction = 4;
            break;
            
        case 'S': /* Stop */
            Motor_Control(0, 0);
            car_direction = 0;
            break;
            
        case '1': /* Low Speed */
            car_speed = 1;
            Motor_Control(car_direction, car_speed);
            break;
            
        case '2': /* Medium Speed */
            car_speed = 2;
            Motor_Control(car_direction, car_speed);
            break;
            
        case '3': /* High Speed */
            car_speed = 3;
            Motor_Control(car_direction, car_speed);
            break;
            
        case 'M': /* Switch Mode */
            car_mode = (car_mode + 1) % 3;
            Motor_Control(0, 0);  /* Stop */
            break;
            
        default:
            break;
    }
}

/**
 * @brief  USART2 Interrupt Handler
 * @retval None
 */
void USART2_IRQHandler(void)
{
    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
    {
        uint8_t cmd = (uint8_t)USART_ReceiveData(USART2);
        Bluetooth_Control(cmd);
        USART_ClearITPendingBit(USART2, USART_IT_RXNE);
    }
}

/**
 * @brief  Redirect printf output
 * @retval Character
 */
#ifdef __GNUC__
int __io_putchar(int ch)
#else
int fputc(int ch, FILE *f)
#endif
{
    USART_SendChar((uint8_t)ch);
    return ch;
}
