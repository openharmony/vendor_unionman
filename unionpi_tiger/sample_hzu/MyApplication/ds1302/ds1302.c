#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include "securec.h"
#include "um_gpio.h"
#include "ds1302.h"


// Write Register Address
#define DS1302_SEC				0x80
#define DS1302_MIN				0x82
#define DS1302_HOUR				0x84
#define DS1302_DATE				0x86
#define DS1302_MONTH			0x88
#define DS1302_DAY				0x8a
#define DS1302_YEAR				0x8c
#define DS1302_CONTROL		0x8e
#define DS1302_CHARGER		0x90 					 
#define DS1302_CLKBURST		0xbe
#define DS1302_RAMBURST 	0xfe

#define RAMSIZE 					0x31	// Ram Size in bytes
#define DS1302_RAMSTART		0xc0 	// First Address


#define HEX2BCD(v)	((v) % 10 + (v) / 10 * 16)
#define BCD2HEX(v)	((v) % 16 + (v) / 16 * 10)

// GPIO Pins
#define DS1302_GPIO	GPIOI
#define DS1302_SCLK	UM_GPIO_09
#define DS1302_SDA	UM_GPIO_08
#define DS1302_RST	UM_GPIO_06

uint8_t buf[] = {0, 24, 12, 1, 7, 30, 0, 1};
/* Clock signal need to be at least 1 micro second wide, those delays are generated with DWT		*/
/* More info:  https://www.carminenoviello.com/2015/09/04/precisely-measure-microseconds-stm32/ */



// SDA Write(output) Mode
static void writeSDA(void) {
	int getValue = -1;
	// 判断gpio口是否已经导出，如未导出则执行对应函数
    UM_GPIO_IsExport(DS1302_SDA, &getValue);
    if (getValue == UM_GPIO_NOT_EXPORT) {
        UM_GPIO_Export(DS1302_SDA, UM_GPIO_EXPORTED);
    }
	UM_GPIO_SetDirection(DS1302_SDA, UM_GPIO_DIRECTION_OUT);
	
}

// SDA Read(input) Mode
static void readSDA(void) {
	int getValue = -1;
	// 判断gpio口是否已经导出，如未导出则执行对应函数
    UM_GPIO_IsExport(DS1302_SDA, &getValue);
    if (getValue == UM_GPIO_NOT_EXPORT) {
        UM_GPIO_Export(DS1302_SDA, UM_GPIO_EXPORTED);
    }
	UM_GPIO_SetDirection(DS1302_SDA, UM_GPIO_DIRECTION_IN);
}

/* Sends an address or command */
static void DS1302_SendCmd(uint8_t cmd) {
	uint8_t i;
	for (i = 0; i < 8; i ++) 
	{	
//		DS1302_SDA = (bit)(addr & 1); 
		if(cmd&1)
		{
			UM_GPIO_SetValue(DS1302_SDA, UM_GPIO_HIGH_LEVE);
		}
		else
		{
			UM_GPIO_SetValue(DS1302_SDA, UM_GPIO_LOW_LEVE);
		}
//		DS1302_SCK = 1;
		UM_GPIO_SetValue(DS1302_SCLK, UM_GPIO_HIGH_LEVE);
		usleep(1);
//		DS1302_SCK = 0;
		UM_GPIO_SetValue(DS1302_SCLK, UM_GPIO_LOW_LEVE);
		usleep(1);
		cmd >>= 1;
	}
}


/* Writes a byte to 'addr' */
static void DS1302_WriteByte(uint8_t addr, uint8_t d)
{
	uint8_t i;

//	DS1302_RST = 1;
	UM_GPIO_SetValue(DS1302_RST, UM_GPIO_HIGH_LEVE);
	//addr = addr & 0xFE;
	DS1302_SendCmd(addr);	// Sends address
	
	for (i = 0; i < 8; i ++) 
	{
//		DS1302_SDA = (bit)(d & 1);
		if(d&1)
		{
			UM_GPIO_SetValue(DS1302_SDA, UM_GPIO_HIGH_LEVE);
		}
		else
		{
			UM_GPIO_SetValue(DS1302_SDA, UM_GPIO_LOW_LEVE);
		}
//		DS1302_SCK = 1;
		UM_GPIO_SetValue(DS1302_SCLK, UM_GPIO_HIGH_LEVE);
		usleep(1);
//		DS1302_SCK = 0;
		UM_GPIO_SetValue(DS1302_SCLK, UM_GPIO_LOW_LEVE);
		usleep(1);
		d >>= 1;
	}
	
//	DS1302_RST = 0;
	UM_GPIO_SetValue(DS1302_RST, UM_GPIO_LOW_LEVE);
	//	DS1302_SDA = 0;
	UM_GPIO_SetValue(DS1302_SDA, UM_GPIO_LOW_LEVE);
}


/* Sends 'cmd' command and writes in burst mode 'len' bytes from 'temp' */
static void DS1302_WriteBurst(uint8_t cmd, uint8_t len, uint8_t * temp)
{
	uint8_t i, j;
	
	DS1302_WriteByte(DS1302_CONTROL,0x00);			// Disable write protection

//	DS1302_RST = 1;
	UM_GPIO_SetValue(DS1302_RST, UM_GPIO_HIGH_LEVE);
	DS1302_SendCmd(cmd);	// Sends burst write command
	
	for(j = 0; j < len; j++) {
		for (i = 0; i < 8; i ++) 
		{
//			DS1302_SDA = (bit)(d & 1);
			if(temp[j]&1)
			{
				UM_GPIO_SetValue(DS1302_SDA, UM_GPIO_HIGH_LEVE);
			}
			else
			{
				UM_GPIO_SetValue(DS1302_SDA, UM_GPIO_LOW_LEVE);
			}
	//		DS1302_SCK = 1;
			UM_GPIO_SetValue(DS1302_SCLK, UM_GPIO_HIGH_LEVE);
			usleep(1);
	//		DS1302_SCK = 0;
			UM_GPIO_SetValue(DS1302_SCLK, UM_GPIO_LOW_LEVE);
			usleep(1);
			temp[j] >>= 1;
		}
	}
	
//	DS1302_RST = 0;
	UM_GPIO_SetValue(DS1302_RST, UM_GPIO_LOW_LEVE);
	//	DS1302_SDA = 0;
	UM_GPIO_SetValue(DS1302_SDA, UM_GPIO_LOW_LEVE);
	
	DS1302_WriteByte(DS1302_CONTROL,0x80);			// Enable write protection
}


/* Reads a byte from addr */
static uint8_t DS1302_ReadByte(uint8_t addr) 
{
	uint8_t i;
	uint8_t temp = 0;
	int value = -1;
//	DS1302_RST = 1;
	UM_GPIO_SetValue(DS1302_RST, UM_GPIO_HIGH_LEVE);
	addr = addr | 0x01; // Generate Read Address

	DS1302_SendCmd(addr);	// Sends address
	
	readSDA();
	for (i = 0; i < 8; i ++) 
	{
		temp >>= 1;
//		if(DS1302_SDA)
		UM_GPIO_GetValue(DS1302_SDA, &value);
		if(value)
			temp |= 0x80;
//		DS1302_SCK = 1;
		UM_GPIO_SetValue(DS1302_SCLK, UM_GPIO_HIGH_LEVE);
		usleep(1);
//		DS1302_SCK = 0;
		UM_GPIO_SetValue(DS1302_SCLK, UM_GPIO_LOW_LEVE);
		usleep(1);
	}
	writeSDA();

//	DS1302_RST = 0;
	UM_GPIO_SetValue(DS1302_RST, UM_GPIO_LOW_LEVE);
//	DS1302_SDA = 0;
	UM_GPIO_SetValue(DS1302_SDA, UM_GPIO_LOW_LEVE);
	return temp;
}


/* Sends 'cmd' command and reads in burst mode 'len' bytes into 'temp' */
static void DS1302_ReadBurst(uint8_t cmd, uint8_t len, uint8_t * temp) 
{
	uint8_t i, j;

//	DS1302_RST = 1;
	UM_GPIO_SetValue(DS1302_RST, UM_GPIO_HIGH_LEVE);
	int value =-1;
	cmd = cmd | 0x01; // Generate read command

	DS1302_SendCmd(cmd);	// Sends burst read command
	
	readSDA();
	for (j = 0; j < len; j ++) {
		temp[j] = 0;
		for (i = 0; i < 8; i ++) 
		{
			temp[j] >>= 1;
			//			if(DS1302_SDA)
			UM_GPIO_GetValue(DS1302_SDA, &value);
			if(value)
				temp[j] |= 0x80;

	//		DS1302_SCK = 1;
			UM_GPIO_SetValue(DS1302_SCLK, UM_GPIO_HIGH_LEVE);
			usleep(1);
	//		DS1302_SCK = 0;
			UM_GPIO_SetValue(DS1302_SCLK, UM_GPIO_LOW_LEVE);
			usleep(1);

		}
	}
	writeSDA();

//	DS1302_RST = 0;
	UM_GPIO_SetValue(DS1302_RST, UM_GPIO_LOW_LEVE);
	UM_GPIO_SetValue(DS1302_SDA, UM_GPIO_LOW_LEVE);
}


/* Writes time byte by byte from 'buf' */
void DS1302_WriteTime(uint8_t *buf) 
{	
	DS1302_WriteByte(DS1302_CONTROL,0x00);			// Disable write protection
	usleep(1);
	DS1302_WriteByte(DS1302_SEC,0x80);
	DS1302_WriteByte(DS1302_YEAR,HEX2BCD(buf[1]));
	DS1302_WriteByte(DS1302_MONTH,HEX2BCD(buf[2]));
	DS1302_WriteByte(DS1302_DATE,HEX2BCD(buf[3]));
	DS1302_WriteByte(DS1302_HOUR,HEX2BCD(buf[4]));
	DS1302_WriteByte(DS1302_MIN,HEX2BCD(buf[5]));
	DS1302_WriteByte(DS1302_SEC,HEX2BCD(buf[6]));
	DS1302_WriteByte(DS1302_DAY,HEX2BCD(buf[7]));
	DS1302_WriteByte(DS1302_CONTROL,0x80);			// Enable write protection
	usleep(1);
}


/* Reads time byte by byte to 'buf' */
void DS1302_ReadTime(uint8_t *buf)  
{ 
   	uint8_t tmp;
	
	tmp = DS1302_ReadByte(DS1302_YEAR); 	
	buf[1] = BCD2HEX(tmp);		 
	tmp = DS1302_ReadByte(DS1302_MONTH); 	
	buf[2] = BCD2HEX(tmp);	 
	tmp = DS1302_ReadByte(DS1302_DATE); 	
	buf[3] = BCD2HEX(tmp);
	tmp = DS1302_ReadByte(DS1302_HOUR);		
	buf[4] = BCD2HEX(tmp);
	tmp = DS1302_ReadByte(DS1302_MIN);		
	buf[5] = BCD2HEX(tmp); 
	tmp = DS1302_ReadByte((DS1302_SEC))&0x7F; 
	buf[6] = BCD2HEX(tmp);
	tmp = DS1302_ReadByte(DS1302_DAY);		
	buf[7] = BCD2HEX(tmp);
}

/* Initialization */
void DS1302_Init(void)
{
	int getValue = -1;
	// 判断gpio口是否已经导出，如未导出则执行对应函数
    UM_GPIO_IsExport(DS1302_SDA, &getValue);
    if (getValue == UM_GPIO_NOT_EXPORT) {
        UM_GPIO_Export(DS1302_SDA, UM_GPIO_EXPORTED);
    }
	UM_GPIO_SetDirection(DS1302_SDA, UM_GPIO_DIRECTION_OUT);

	// 判断gpio口是否已经导出，如未导出则执行对应函数
    UM_GPIO_IsExport(DS1302_SCLK, &getValue);
    if (getValue == UM_GPIO_NOT_EXPORT) {
        UM_GPIO_Export(DS1302_SCLK, UM_GPIO_EXPORTED);
    }
	UM_GPIO_SetDirection(DS1302_SCLK, UM_GPIO_DIRECTION_OUT);

	// 判断gpio口是否已经导出，如未导出则执行对应函数
    UM_GPIO_IsExport(DS1302_RST, &getValue);
    if (getValue == UM_GPIO_NOT_EXPORT) {
        UM_GPIO_Export(DS1302_RST, UM_GPIO_EXPORTED);
    }
	UM_GPIO_SetDirection(DS1302_RST, UM_GPIO_DIRECTION_OUT);

	DS1302_WriteByte(DS1302_CHARGER,0x00);			// Disable Trickle Charger 
		
//	DS1302_RST = 0;
	UM_GPIO_SetValue(DS1302_RST, UM_GPIO_LOW_LEVE);
//	DS1302_SCK = 0;
	UM_GPIO_SetValue(DS1302_SCLK, UM_GPIO_LOW_LEVE);

}

/* Writes 'val' to ram address 'addr' */
/* Ram addresses range from 0 to 30 */
void DS1302_WriteRam(uint8_t addr, uint8_t val) {
	DS1302_WriteByte(DS1302_CONTROL,0x00);			// Disable write protection
	usleep(1);
	if (addr >= RAMSIZE) {
		return;
	}
	
	DS1302_WriteByte(DS1302_RAMSTART + (2 * addr), val);	
	
	DS1302_WriteByte(DS1302_CONTROL,0x80);			// Enable write protection
	usleep(1);
}

/* Reads ram address 'addr' */
uint8_t DS1302_ReadRam(uint8_t addr) {
	if (addr >= RAMSIZE) {
		return 0;
	}
	
	return DS1302_ReadByte(DS1302_RAMSTART + (2 * addr));	
}


/* Clears the entire ram writing 0 */
void DS1302_ClearRam(void) {
	uint8_t i;
	for(i=0; i< RAMSIZE; i++){
		DS1302_WriteRam(i,0x00);
	}
}


/* Reads time in burst mode, includes control byte */
void DS1302_ReadTimeBurst() {
	uint8_t temp[8] = {0, 0, 0, 0, 0, 0, 0, 0};
	
	DS1302_ReadBurst(DS1302_CLKBURST, 8, temp); 
	
	buf[1] = BCD2HEX(temp[6]);	// Year
	buf[2] = BCD2HEX(temp[4]);	// Month
	buf[3] = BCD2HEX(temp[3]);	// Date
	buf[4] = BCD2HEX(temp[2]);	// Hour
	buf[5] = BCD2HEX(temp[1]);	// Min
	buf[6] = BCD2HEX(temp[0]);	// Sec
	buf[7] = BCD2HEX(temp[5]);	// Day
	buf[0] = temp[7]; 					// Control
}


/* Writes time in burst mode, includes control byte */
void DS1302_WriteTimeBurst() {
	uint8_t temp[8];
	
	temp[0]=HEX2BCD(buf[6]);	// Sec
	temp[1]=HEX2BCD(buf[5]);	// Min
	temp[2]=HEX2BCD(buf[4]);	// Hour
	temp[3]=HEX2BCD(buf[3]);	// Date
	temp[4]=HEX2BCD(buf[2]);	// Month
	temp[5]=HEX2BCD(buf[7]);	// Day
	temp[6]=HEX2BCD(buf[1]);	// Year
	temp[7]=buf[0];						// Control
	
	DS1302_WriteBurst(DS1302_CLKBURST, 8, temp); 
}

/* Reads ram in burst mode 'len' bytes into 'buf' */
void DS1302_ReadRamBurst(uint8_t len, uint8_t * buf) {
	uint8_t i;
	if(len <= 0) {
		return;
	}
	if (len > RAMSIZE) {
		len = RAMSIZE;
	}
	for(i = 0; i < len; i++) {
		buf[i] = 0;
	}
	DS1302_ReadBurst(DS1302_RAMBURST, len, buf);	
}

/* Writes ram in burst mode 'len' bytes from 'buf' */
void DS1302_WriteRamBurst(uint8_t len, uint8_t * buf) {
	if(len <= 0) {
		return;
	}
	if (len > RAMSIZE) {
		len = RAMSIZE;
	}
	DS1302_WriteBurst(DS1302_RAMBURST, len, buf);
}
