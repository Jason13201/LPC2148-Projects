#include <lpc214x.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

void delay_ms(uint16_t j) /* Function for delay in milliseconds */
{
    uint16_t x,i;
	for(i=0;i<j;i++)
	{
    for(x=0; x<6000; x++);    /* loop to generate 1 millisecond delay with Cclk = 60MHz */
	}
}

void LCD_CMD(char command)
{
	IO0PIN = ( (IO0PIN & 0xFFFF00FF) | ((command & 0xF0)<<8) ); /* Upper nibble of command */
	IO0SET = 0x00000040; /* EN = 1 */
	IO0CLR = 0x00000030; /* RS = 0, RW = 0 */
	delay_ms(5);
	IO0CLR = 0x00000040; /* EN = 0, RS and RW unchanged(i.e. RS = RW = 0)	 */
	delay_ms(5);
	IO0PIN = ( (IO0PIN & 0xFFFF00FF) | ((command & 0x0F)<<12) ); /* Lower nibble of command */
	IO0SET = 0x00000040; /* EN = 1 */
	IO0CLR = 0x00000030; /* RS = 0, RW = 0 */
	delay_ms(5);
	IO0CLR = 0x00000040; /* EN = 0, RS and RW unchanged(i.e. RS = RW = 0)	 */
	delay_ms(5);
	
}

void LCD_INIT(void)
{
	IO0DIR = 0x0000FFF0; /* P0.12 to P0.15 LCD Data. P0.4,5,6 as RS RW and EN */
	delay_ms(20);
	LCD_CMD(0x02);  /* Initialize lcd in 4-bit mode */
	LCD_CMD(0x28);  /* 2 lines */
	LCD_CMD(0x0C);   /* Display on cursor off */
	LCD_CMD(0x06);  /* Auto increment cursor */
	LCD_CMD(0x01);   /* Display clear */
	LCD_CMD(0x80);  /* First line first position */
}

void LCD_STRING (char* msg)
{
	uint8_t i=0;
	while(msg[i]!=0)
	{
		IO0PIN = ( (IO0PIN & 0xFFFF00FF) | ((msg[i] & 0xF0)<<8) );
		IO0SET = 0x00000050; /* RS = 1, EN = 1 */
		IO0CLR = 0x00000020; /* RW = 0 */
		delay_ms(2);
		IO0CLR = 0x00000040; /* EN = 0, RS and RW unchanged(i.e. RS = 1, RW = 0) */
		delay_ms(5);
		IO0PIN = ( (IO0PIN & 0xFFFF00FF) | ((msg[i] & 0x0F)<<12) );
		IO0SET = 0x00000050; /* RS = 1, EN = 1 */
		IO0CLR = 0x00000020; /* RW = 0 */
		delay_ms(2);
		IO0CLR = 0x00000040; /* EN = 0, RS and RW unchanged(i.e. RS = 1, RW = 0) */
		delay_ms(5);
		i++;
	}
}

void LCD_CHAR (char msg)
{
		IO0PIN = ( (IO0PIN & 0xFFFF00FF) | ((msg & 0xF0)<<8) );
		IO0SET = 0x00000050; /* RS = 1, EN = 1 */
		IO0CLR = 0x00000020; /* RW = 0 */
		delay_ms(2);
		IO0CLR = 0x00000040; /* EN = 0, RS and RW unchanged(i.e. RS = 1, RW = 0) */
		delay_ms(5);
		IO0PIN = ( (IO0PIN & 0xFFFF00FF) | ((msg & 0x0F)<<12) );
		IO0SET = 0x00000050; /* RS = 1, EN = 1 */
		IO0CLR = 0x00000020; /* RW = 0 */
		delay_ms(2);
		IO0CLR = 0x00000040; /* EN = 0, RS and RW unchanged(i.e. RS = 1, RW = 0) */
		delay_ms(5);
}

unsigned char flag=0;

void rtc_int(void)__irq{
	ILR = 0X01;
	flag = 1;
	VICVectAddr = 0X00000000;
}



void RTC_INIT(){
	ILR = 0X01;
	CCR  = 0X13;
	CCR =  0X11;
	CIIR = 0X01;
	SEC = 0X05;
	MIN = 0X12;
	HOUR = 0X06;
	VICIntEnable = 0x00002000;
	VICVectCntl0 = 0x0000002D;
	VICVectAddr0 = (unsigned)rtc_int;
}

int tzOffset = 0;
int tzOffsetHr = 0;
int tzOffsetMin = 0;
char timestring[8];
void showTime() {
	int ofsettedMin = (MIN + tzOffsetMin + 60) % 60;
	int ofsettedHr = (HOUR + tzOffsetHr + 12) % 12;
	sprintf(timestring,"%02d:%02d:%02d", ofsettedHr, ofsettedMin, SEC);
	LCD_CMD(0x86);
	LCD_STRING(timestring);
}

char gmtString[6];
void showTimeZone() {
	LCD_CMD(0xC3);
	sprintf(gmtString, "%c%02d:%02d", tzOffset >= 0 ? '+' : '-', (tzOffset >= 0 ? tzOffset : -tzOffset ) / 2, tzOffset % 2 ? 30 : 0);
	LCD_STRING(gmtString);
}


int main(void){
	LCD_INIT();
	RTC_INIT();
	LCD_STRING("Time: ");
	showTime();
	LCD_CMD(0xC0);
	LCD_STRING("GMT");
	showTimeZone();
	IO1DIR &= ~(1 << 24);
	IO1DIR &= ~(1 << 26);
	while(1){
		if(flag){
			showTime();
			flag=0;
		}
		if ((IO1PIN & (1 << 24)) == 0) {
			tzOffset -= 1;
			if (tzOffsetMin == 0) {
				tzOffsetMin = 30;
				tzOffsetHr -= 1;
			} else {
				tzOffsetMin = 0;
			}
			showTime();
			showTimeZone();
			delay_ms(250);
		}
		if ((IO1PIN & (1 << 26)) == 0) {
			tzOffset += 1;
			if (tzOffsetMin == 0) {
				tzOffsetMin = 30;
			} else {
				tzOffsetMin = 0;
				tzOffsetHr += 1;
			}
			showTime();
			showTimeZone();
			delay_ms(250);
		}
  }
}
