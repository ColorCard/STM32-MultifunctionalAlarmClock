#include "stm32f10x.h"
#include "Blink_Display.h"

extern uint8_t blink_flag;  // 引入全局变量 blink_flag，用于控制闪烁状态

void Timer_Init(void) {
    // 1. 开启 TIM2 时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    // 2. 配置定时器基础设置
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_Period = 4999;              // 设置自动重装载值，配合时钟分频器可以得到1Hz频率
    TIM_TimeBaseStructure.TIM_Prescaler = 7199;           // 预分频值，使计数频率为 10KHz (72MHz / 7200)
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    // 3. 清除更新中断标志位，启用定时器更新中断
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);            // 使能更新中断

    // 4. 启动定时器
    TIM_Cmd(TIM2, ENABLE);

    // 5. 配置中断优先级并开启中断
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;       // 定时器2中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;       // 使能中断
    NVIC_Init(&NVIC_InitStructure);
}

// 定时器2中断服务函数
void TIM2_IRQHandler(void) {
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) {  // 检查更新中断
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);       // 清除中断标志

        blink_flag = !blink_flag;                         // 翻转闪烁标志，控制显示闪烁
    }
}