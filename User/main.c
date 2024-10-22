#include "stm32f10x.h"                  // Device header
#include "OLED.h"
#include "LED.h"
#include "MyRTC.h"
#include "Timer.h"
#include "Key.h"
#include "Blink_Display.h"
#include "Buzzer.h"
#include "Serial.h"
#include "ds18b20.h"
#include "AD.h"


uint8_t set_state = 0;      // 0: 正常显示状态，非0: 时间设置状态
uint8_t blink_flag = 0;     // 闪烁标志，用于控制显示闪烁
uint8_t Alarm_Hour = 7, Alarm_Minute = 30;  // 闹钟时间初始化
float Alarm_Temperature = 30.0;             // 温度报警阈值初始化
uint8_t Alarm_Flag = 0, TempAlarm_Flag = 0; // 报警标志位
uint16_t Temp;
uint16_t ADValue;			//定义AD值变量
float Voltage;				//定义电压变量

int main(void) {
    /*模块初始化*/
	LED_Init();
    Serial_Init();
    OLED_Init();        //OLED初始化
    MyRTC_Init();        //RTC初始化
    Key_Init();         // 按键初始化
    Buzzer_Init();
    Timer3_Init();      //灯闪烁和鸣叫中断
    Timer_Init();       // 初始化定时器，用于控制闪烁
    DS18B20_Init();
	AD_Init();

    


	BKP_WriteBackupRegister(BKP_DR1, 0x0000);  // 清除备份寄存器标志位

    /*显示静态字符串*/
    OLED_ShowString(1, 1, "Date:XXXX-XX-XX");
    OLED_ShowString(2, 1, "Time:XX:XX:XX");
    OLED_ShowString(3, 1, "Alarm:XX:XX");
    OLED_ShowString(4, 1, "    C Warn:XX.XC");
	

	


    while (1) {
        Temp=DS18B20_ReadTemp();
        Key_Scan();  // 按键扫描
		
		ADValue = AD_GetValue();					//获取AD转换的值
		Voltage = (float)ADValue / 4095 * 5;		//将AD值线性变换到0~3.3的范围，表示电压
		
		//Serial_Printf("%f",Voltage);

		


        if (set_state == 0) {  // 如果不在设置状态，读取RTC时间
            MyRTC_ReadTime();  // RTC读取时间，最新的时间存储到MyRTC_Time数组中

            // 检查闹钟
            if (MyRTC_Time[3] == Alarm_Hour && MyRTC_Time[4] == Alarm_Minute) {
                Alarm_Flag = 1;
            } else {
                Alarm_Flag = 0;
            }


			//Serial_Printf("Test：%lf",Temp/10.0);

            // 检查温度报警
            if ((Temp/10.0) > Alarm_Temperature) {
                TempAlarm_Flag = 1;
            } else {
                TempAlarm_Flag = 0;
            }

            // 报警处理
            if (Alarm_Flag || TempAlarm_Flag) {

            } else {
                Buzzer_OFF(); // 关闭蜂鸣器
                LED_OFF();    // 关闭LED
            }
        }

        if (set_state == 0) {  // 正常显示时间
            OLED_ShowNum(1, 6, MyRTC_Time[0], 4);   // 显示年
            OLED_ShowNum(1, 11, MyRTC_Time[1], 2);  // 显示月
            OLED_ShowNum(1, 14, MyRTC_Time[2], 2);  // 显示日
            OLED_ShowNum(2, 6, MyRTC_Time[3], 2);   // 显示时
            OLED_ShowNum(2, 9, MyRTC_Time[4], 2);   // 显示分
            OLED_ShowNum(2, 12, MyRTC_Time[5], 2);  // 显示秒
			OLED_ShowString(2, 15, "W");
            OLED_ShowNum(2, 16, MyRTC_Time[6], 1);  
            OLED_ShowNum(3, 7, Alarm_Hour, 2);      // 显示闹钟小时
            OLED_ShowNum(3, 10, Alarm_Minute, 2);   // 显示闹钟分钟
			OLED_ShowNum(3, 13, Voltage, 1);				//显示AD值
			OLED_ShowString(3, 14, ".");
            OLED_ShowNum(3, 15, (uint16_t)(Voltage * 100) % 100, 2);//电压
            //OLED_ShowNum(4, 1, Temp, 2);//实时温度
            OLED_ShowNum(4, 1,(int)(Temp/10.0 * 100 + 0.5) / 100.0, 2);
            OLED_ShowString(4, 3, ".");
            OLED_ShowNum(4, 4, (int)((Temp/10.0 - (int)Alarm_Temperature) * 100),1);// 显示实时温度


            OLED_ShowNum(4, 12, (int)(Alarm_Temperature * 100 + 0.5) / 100.0, 2);
			OLED_ShowString(4, 14, ".");
			OLED_ShowNum(4, 15, (int)((Alarm_Temperature - (int)Alarm_Temperature) * 10),1);// 显示温度报警值
			OLED_ShowString(4, 16, "C");
//			OLED_ShowNum(4, 12, Temp, 3);
        } else {
            Blink_Display();  // 在设置状态时处理闪烁显示
        }
    }
}



