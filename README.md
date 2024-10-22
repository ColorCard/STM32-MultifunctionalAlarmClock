## STM32f103RCT6 的多功能数字钟


##### 本项目遵守 [MIT License](LICENSE) 开源协议，有帮助的话留下一个 **Star ⭐**️ 吧 🥳

---

### 本项目基本功能：
1. **时钟的基准用 STM32F103RCT6 自带的 RTC 实现。**<br>[MyRTC.c](System%2FMyRTC.c)


2. **实现了按钮设置功能。** <br>采用矩阵键盘设计，三个按键的功能分别为：`设置`、`加`、`减`（`WAKEUP`、`KEY0`、`KEY1`)。 <br>设定时间、日期、星期时，按下设定键，相应的参数会闪动，即一亮一灭，提示正的修改的数据，每按一下设置键，设定的项目就会变化，顺序为：`秒` `分` `时` `日` `月` `年` `闹铃分` `闹铃时` `报警温度`，（显示屏选择使用0.96寸OLED）。<br>顺序设定完后，再按一下设定键，就退出设定状态，回到正显示状态。<br>[Key.c](Hardware%2FKey.c)<br>[Blink_Display.c](Hardware%2FBlink_Display.c) 



3. **用 DS18B20 测量温度并显示。** 精确到小数点后一位。<br>[ds18b20.c](Hardware%2Fds18b20.c)


4. **通过无线串口设置和查看时间日期。<br>** 如无线串口电脑端发送`t12:30:00`，则设置时间；如串口发送`d2016-08-12` ，则设置日期。<br>如电脑串口端发送 `t?`，则时钟返回当前实际时间给电脑；如发送`d?`，则时钟返回当前实际日期给电脑。<br>[Serial.c](Hardware%2FSerial.c)


5. **闹铃功能、温度报警功能。**<br>时间到/温度超过设置值，蜂鸣器发生“嘀嘀”声，LED会进行闪烁。<br>[Buzzer.c](Hardware%2FBuzzer.c)<br>[LED.c](Hardware%2FLED.c)<br>[Timer.c](Hardware%2FTimer.c)


6. **电压显示功能。**<br>通过ADC采集电池电压，并且能够在 OLED 上实时显示当前电池电压大小（使用了电阻分压）。<br>[AD.c](Hardware%2FAD.c)



---

### 电路图片参考：<br>
![example.jpg](example.jpg)

---

### **核心组件选择与采购** ：
- STM32F103RCT6 微控制器。
- 0.96寸 OLED 显示屏。
- 3 个按键。
- DS18B20 温度传感器。
- 蜂鸣器。
- 电阻（2kΩ 和 15kΩ 和 33kΩ）。
- 无线串口模块 NRF24L01。
- 电线电路。