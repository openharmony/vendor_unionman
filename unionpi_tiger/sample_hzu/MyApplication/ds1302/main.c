#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include "securec.h"
#include "um_gpio.h"
#include "ds1302.h"



int main()
{
    DS1302_Init();
    DS1302_WriteTimeBurst();
    while(1)
    {
        DS1302_ReadTimeBurst();
        printf("time is %d - %d - %d  %d : %d : %d\n",buf[1],buf[2],buf[3],buf[4],buf[5],buf[6]);
        usleep(1000*1000);
    }
    return 0;
}