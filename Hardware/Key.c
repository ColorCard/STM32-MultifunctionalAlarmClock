#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "MyRTC.h"



extern uint8_t set_state;    // 从主程序引入状态机变量
extern uint8_t blink_flag;   // 引入闪烁标志
extern uint16_t MyRTC_Time[]; // 引入时间数组
extern uint8_t Alarm_Hour, Alarm_Minute;  // 闹钟时间初始化
extern float Alarm_Temperature;

void Key_AddHandler(void);
void Key_SubHandler(void);

void Key_Init(void)
{
	/*开启时钟*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	/*GPIO初始化*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_3 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}


#define KEY_SET     GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1)  // 设置按键
#define KEY_ADD     GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_3)  // 加按键
#define KEY_SUB     GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_5)  // 减按键


void Key_Scan(void) {
    static uint8_t key_set_last = 1, key_add_last = 1, key_sub_last = 1;

    // 设置按键：切换调整模式
    if (key_set_last && !KEY_SET) {
        key_set_last = 0;
        set_state++;
        if (set_state > 9) {           // 设置状态超过9项后，退出设置模式
            set_state = 0;
            MyRTC_SetTime();           // 保存时间到RTC
        }
    } else if (!key_set_last && KEY_SET) {
        key_set_last = 1;
    }

    // 加按键：在设置模式下加
    if (key_add_last && !KEY_ADD && set_state != 0) {
        key_add_last = 0;
        Key_AddHandler();
    } else if (!key_add_last && KEY_ADD) {
        key_add_last = 1;
    }

    // 减按键：在设置模式下减
    if (key_sub_last && !KEY_SUB && set_state != 0) {
        key_sub_last = 0;
        Key_SubHandler();
    } else if (!key_sub_last && KEY_SUB) {
        key_sub_last = 1;
    }
}
// 判断是否为闰年
int IsLeapYear(int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

// 获取某个月份的天数
int GetDaysInMonth(int year, int month) {
    switch (month) {
        case 1: case 3: case 5: case 7: case 8: case 10: case 12:
            return 31;  // 31天的月份
        case 4: case 6: case 9: case 11:
            return 30;  // 30天的月份
        case 2:
            return IsLeapYear(year) ? 29 : 28;  // 闰年29天，平年28天
        default:
            return 30;  // 默认防护（虽然不应该出现）
    }
}

void Key_AddHandler(void) {
    switch (set_state) {
        case 1: MyRTC_Time[5] = (MyRTC_Time[5] + 1) % 60; break;  // 秒
        case 2: MyRTC_Time[4] = (MyRTC_Time[4] + 1) % 60; break;  // 分
        case 3: MyRTC_Time[3] = (MyRTC_Time[3] + 1) % 24; break;  // 时
        case 4:  // 调整日期
            if (++MyRTC_Time[2] > GetDaysInMonth(MyRTC_Time[0], MyRTC_Time[1])) {
                MyRTC_Time[2] = 1;  // 超出当月天数时重置为1
            }
            break;
        case 5:  // 调整月份
            MyRTC_Time[1] = (MyRTC_Time[1] % 12) + 1;  // 月份范围在1-12
            if (MyRTC_Time[2] > GetDaysInMonth(MyRTC_Time[0], MyRTC_Time[1])) {
                MyRTC_Time[2] = GetDaysInMonth(MyRTC_Time[0], MyRTC_Time[1]);  // 超出当前月的天数时，调整为该月最后一天
            }
            break;
        case 6:  // 调整年份
            MyRTC_Time[0] = (MyRTC_Time[0] + 1) % 2100;  // 年份范围0-2099
            if (MyRTC_Time[2] > GetDaysInMonth(MyRTC_Time[0], MyRTC_Time[1])) {
                MyRTC_Time[2] = GetDaysInMonth(MyRTC_Time[0], MyRTC_Time[1]);  // 调整日期为当前月份的最大天数
            }
            break;
        case 7: Alarm_Hour = (Alarm_Hour + 1) % 24; break;    // 闹钟小时
        case 8: Alarm_Minute = (Alarm_Minute + 1) % 60; break; // 闹钟分钟
        case 9: Alarm_Temperature += 0.5; break;               // 报警温度增加
    }
}

void Key_SubHandler(void) {
    switch (set_state) {
        case 1: MyRTC_Time[5] = (MyRTC_Time[5] - 1 + 60) % 60; break;  // 秒
        case 2: MyRTC_Time[4] = (MyRTC_Time[4] - 1 + 60) % 60; break;  // 分
        case 3: MyRTC_Time[3] = (MyRTC_Time[3] - 1 + 24) % 24; break;  // 时
        case 4:  // 调整日期
            if (--MyRTC_Time[2] < 1) {
                MyRTC_Time[1] = (MyRTC_Time[1] == 1) ? 12 : MyRTC_Time[1] - 1;  // 如果日期减到0，月份减1
                MyRTC_Time[2] = GetDaysInMonth(MyRTC_Time[0], MyRTC_Time[1]);  // 设置为新月份的最后一天
            }
            break;
        case 5:  // 调整月份
            MyRTC_Time[1] = (MyRTC_Time[1] == 1) ? 12 : MyRTC_Time[1] - 1;  // 月份范围1-12
            if (MyRTC_Time[2] > GetDaysInMonth(MyRTC_Time[0], MyRTC_Time[1])) {
                MyRTC_Time[2] = GetDaysInMonth(MyRTC_Time[0], MyRTC_Time[1]);  // 超出天数时调整为该月最后一天
            }
            break;
        case 6:  // 调整年份
            MyRTC_Time[0] = (MyRTC_Time[0] - 1 < 2000) ? 2099 : MyRTC_Time[0] - 1;  // 年份范围2000-2099
            if (MyRTC_Time[2] > GetDaysInMonth(MyRTC_Time[0], MyRTC_Time[1])) {
                MyRTC_Time[2] = GetDaysInMonth(MyRTC_Time[0], MyRTC_Time[1]);  // 调整日期为当前月份的最大天数
            }
            break;
        case 7: Alarm_Hour = (Alarm_Hour - 1 + 24) % 24; break;    // 闹钟小时
        case 8: Alarm_Minute = (Alarm_Minute - 1 + 60) % 60; break; // 闹钟分钟
        case 9: Alarm_Temperature -= 0.5; break;                    // 报警温度减少
    }
}

//void Key_AddHandler(void) {
//    switch (set_state) {
//        case 1: MyRTC_Time[5] = (MyRTC_Time[5] + 1) % 60; break;  // 秒
//        case 2: MyRTC_Time[4] = (MyRTC_Time[4] + 1) % 60; break;  // 分
//        case 3: MyRTC_Time[3] = (MyRTC_Time[3] + 1) % 24; break;  // 时
//        case 4: MyRTC_Time[2] = (MyRTC_Time[2] + 1) % 32; break;  // 日
//        case 5: MyRTC_Time[1] = (MyRTC_Time[1] + 1) % 13; break;  // 月
//        case 6: MyRTC_Time[0] = (MyRTC_Time[0] + 1) % 2100; break; // 年
//        case 7: Alarm_Hour = (Alarm_Hour + 1) % 24; break;    // 闹钟小时
//        case 8: Alarm_Minute = (Alarm_Minute + 1) % 60; break;        // 闹钟分钟
//        case 9: Alarm_Temperature += 0.5; break;                  // 报警温度增加
//    }
//}
//
//void Key_SubHandler(void) {
//    switch (set_state) {
//        case 1: MyRTC_Time[5] = (MyRTC_Time[5] - 1 + 60) % 60; break;  // 秒
//        case 2: MyRTC_Time[4] = (MyRTC_Time[4] - 1 + 60) % 60; break;  // 分
//        case 3: MyRTC_Time[3] = (MyRTC_Time[3] - 1 + 24) % 24; break;  // 时
//        case 4: MyRTC_Time[2] = (MyRTC_Time[2] - 1 + 31) % 31; break;  // 日
//        case 5: MyRTC_Time[1] = (MyRTC_Time[1] - 1 + 12) % 12; break;  // 月
//        case 6: MyRTC_Time[0] = (MyRTC_Time[0] - 1 + 100) % 100 + 2000; break; // 年
//        case 7: Alarm_Hour = (Alarm_Hour - 1 + 24) % 24; break;    // 小时
//        case 8: Alarm_Minute = (Alarm_Minute - 1 + 60) % 60; break;        // 闹钟分钟
//        case 9: Alarm_Temperature -= 0.5; break;                      // 报警温度减少
//    }
//}
