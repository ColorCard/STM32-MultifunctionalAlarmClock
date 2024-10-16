#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "Buzzer.h"

extern uint8_t Alarm_Flag;  // 引入全局变量 Alarm_Flag，用于控制闹钟报警

void LED_Turn(void);
void LED_OFF(void);
void LED_ON(void);

void Timer3_Init(void) {
    // 1. 开启 TIM3 时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    // 2. 配置定时器基础设置
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_Period = 4999;              // 设置自动重装载值，配合时钟分频器可以得到1Hz频率
    TIM_TimeBaseStructure.TIM_Prescaler = 7199;           // 预分频值，使计数频率为 10KHz (72MHz / 7200)
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

    // 3. 清除更新中断标志位，启用定时器更新中断
    TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);            // 使能更新中断

    // 4. 启动定时器
    TIM_Cmd(TIM3, ENABLE);

    // 5. 配置中断优先级并开启中断
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;       // 定时器3中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02; // 设置优先级，不与 TIM2 冲突
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;       // 使能中断
    NVIC_Init(&NVIC_InitStructure);
}

void TIM3_IRQHandler(void) {
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) {  // 检查更新中断
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);       // 清除中断标志

        if (Alarm_Flag == 1) {
            LED_Turn(); // LED闪烁表示报警
            Buzzer_Turn();
        } else {
            LED_OFF();    // 关闭LED
        }
    }
}
///**
//  * 函    数：LED初始化
//  * 参    数：无
//  * 返 回 值：无
//  */
void LED_Init(void)
{
	/*开启时钟*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);		//开启GPIOA的时钟

	/*GPIO初始化*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);						//将PA1和PA2引脚初始化为推挽输出

	/*设置GPIO初始化后的默认电平*/
	GPIO_SetBits(GPIOA, GPIO_Pin_7);				//设置PA1和PA2引脚为高电平
}


void LED_Turn(void)
{
    if (GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_7) == 0)		//获取输出寄存器的状态，如果当前引脚输出低电平
    {
        GPIO_SetBits(GPIOA, GPIO_Pin_7);					//则设置PA1引脚为高电平
    }
    else													//否则，即当前引脚输出高电平
    {
        GPIO_ResetBits(GPIOA, GPIO_Pin_7);					//则设置PA1引脚为低电平
    }
}


void LED_ON(void)
{
    GPIO_ResetBits(GPIOA, GPIO_Pin_7);
}


void LED_OFF(void)
{
	GPIO_SetBits(GPIOA, GPIO_Pin_7);		//设置PA1引脚为高电平
}

