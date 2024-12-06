#include"my_um_adc.h"

#include"my_um_gpio.h"

#include<stdio.h>
#include<unistd.h>
//#include<Arduino.h>

// Declaration and initialization of the input pins
//int Analog_Eingang = A0; // X-axis-signal读取传感器
//int Digital_Eingang = 3; // Button
void setup ()
{
 //UM_GPIO_SetDirection(380,0);//设置A0为输入模式，用于读取模拟信号
 UM_GPIO_SetDirection(380,0);// 设置数字引脚3为输入模式，用于读取按钮状态 0:in 1:out
 //begin (9600); // 波特率 with 9600 bps
}
// The program reads the current values at the input pins
// and outputs them at the serial output
void loop ()
{
 float temp;
 int Digital;
 float di=3.0;
 // Current values will be read and converted to voltage
 get_adc_data(1,&temp) ;
 temp = temp * (5.0 / 1023.0);
 get_adc_data(1,&di);
 UM_GPIO_GetValue(380,&Digital);
 //... and outputted here
 printf("Analog voltage value:"); 
 printf("%f",temp); 
 printf("V, ");
 printf("Extreme value:");
 if(Digital==1)
 {
 printf(" reached");
 }
 else
 {
 printf(" not yet reached");
 }
 printf("----------------------------------------------------------------");
 sleep(1);//ms
}
int main()
{
    while(1)
    {
        setup();
        loop();
    }
}