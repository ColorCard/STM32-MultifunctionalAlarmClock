#include "OLED.h"
#include "MyRTC.h"
#include "Key.h"

extern uint8_t set_state;    // 当前设置状态
extern uint8_t blink_flag;   // 闪烁标志
extern uint16_t MyRTC_Time[]; // 时间数组
extern uint8_t Alarm_Hour;   // 闹铃小时
extern uint8_t Alarm_Minute; // 闹铃分钟
extern uint16_t Temp;//实时温度
extern float Alarm_Temperature;  // 报警温度


void Blink_Display(void) {
    if (blink_flag) {
		OLED_ShowString(2, 16, "?");
        switch (set_state) {
            case 1: OLED_ShowString(2, 12, "  "); break; // 清空秒显示
            case 2: OLED_ShowString(2, 9, "  "); break;  // 清空分显示
            case 3: OLED_ShowString(2, 6, "  "); break;  // 清空时显示
            case 4: OLED_ShowString(1, 14, "  "); break; // 清空日显示
            case 5: OLED_ShowString(1, 11, "  "); break; // 清空月显示
            case 6: OLED_ShowString(1, 6, "    "); break; // 清空年显示
            case 7: OLED_ShowString(3, 7, "  "); break;  // 清空闹钟小时显示
            case 8: OLED_ShowString(3, 10, "  "); break;  // 清空闹钟分钟显示
            case 9: OLED_ShowString(4, 12, "     "); break; // 清空报警温度显示
        }
    } else {
        // 恢复显示
        OLED_ShowNum(1, 6, MyRTC_Time[0], 4);   // 显示年
        OLED_ShowNum(1, 11, MyRTC_Time[1], 2);  // 显示月
        OLED_ShowNum(1, 14, MyRTC_Time[2], 2);  // 显示日
        OLED_ShowNum(2, 6, MyRTC_Time[3], 2);   // 显示时
        OLED_ShowNum(2, 9, MyRTC_Time[4], 2);   // 显示分
        OLED_ShowNum(2, 12, MyRTC_Time[5], 2);  // 显示秒
        OLED_ShowNum(3, 7, Alarm_Hour, 2);      // 显示闹钟小时
        OLED_ShowNum(3, 10, Alarm_Minute, 2);    // 显示闹钟分钟

        OLED_ShowNum(4, 1,(int)(Temp/10.0 * 100 + 0.5) / 100.0, 2);
        OLED_ShowString(4, 3, ".");
        OLED_ShowNum(4, 4, (int)((Temp/10.0 - (int)Alarm_Temperature) * 100),1);// 显示实时温度

        OLED_ShowNum(4, 12, (int)Alarm_Temperature, 2);  // 显示报警温度
		OLED_ShowString(4, 14, ".");
		OLED_ShowNum(4, 15, (int)((Alarm_Temperature - (int)Alarm_Temperature) * 10), 1);
		OLED_ShowString(4, 16, "C");
    }
}