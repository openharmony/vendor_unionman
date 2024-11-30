/*
 * Copyright (c) 2022 Unionman Technology Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <securec.h>
#include <unistd.h> 
#include "adc_heart.h"


    double alpha=0.75;
    int period=20;
    double change=0.0;
void delay(int milliseconds);
void delay(int milliseconds) {
    usleep(milliseconds * 10000);  // usleep函数的参数是微秒
}

int main(int argc, char **argv)
{
    static double oldValue=0;
	static double oldChange=0;
    int value = 0;

    while(1){
    if (get_adc_data(ADC_1, &value)>= 0) {
        double rawValue=alpha*oldValue+(1-alpha)*((double)value) ;
        printf("adc:[%d]\n", value);
        printf("rawValue:[%.1f]C\n", rawValue);
        change=rawValue-oldValue;
        oldValue=rawValue;
        oldChange=change;
        delay(period);
    }

    }
    return 0;
}
