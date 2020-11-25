/*
  16x2 LCD (4-bit mode) interfacing with LPC2148(ARM7)
  http://www.electronicwings.com/arm7/lcd-16x2-interfacing-with-lpc2148-4-bit-mode
*/

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

void printBeverage(char * name, char * price) {
	LCD_CMD(0x01);
	LCD_STRING(name);
	LCD_CMD(0xC0);
	LCD_STRING(price);
}

char val_j[3];
int main(void)
{	
	LCD_INIT();
	LCD_STRING("Choose your");
	LCD_CMD(0xC0);
	LCD_STRING("beverage:");
	PINSEL1 = 0x00000000;
	PINSEL2 = 0x00000000;
	IO0DIR |= 0xFFFF0000;
	IO1DIR |= 0x00010000;
	while(1) {
		if ((IO1PIN & (1 << 20)) == 0) {
			printBeverage("Tea", "Rs 10");
			IO0PIN = ( (IO0PIN | 0x00010000) & 0xFFF1FFFF );
			delay_ms(100);
			
			IO0PIN = ( (IO0PIN | 0x00020000) & 0xFFF2FFFF );
			delay_ms(100);
			
			IO0PIN = ( (IO0PIN | 0x00040000) & 0xFFF4FFFF );
			delay_ms(100);
			
			IO0PIN = ( (IO0PIN | 0x00080000) & 0xFFF8FFFF );
			delay_ms(100);
		}
		if ((IO1PIN & (1 << 22)) == 0) {
			printBeverage("Coffee", "Rs 20");
			IO0PIN = ( (IO0PIN | 0x00100000) & 0xFF1FFFFF );
			delay_ms(100);
			
			IO0PIN = ( (IO0PIN | 0x00200000) & 0xFF2FFFFF );
			delay_ms(100);
			
			IO0PIN = ( (IO0PIN | 0x00400000) & 0xFF4FFFFF );
			delay_ms(100);
			
			IO0PIN = ( (IO0PIN | 0x00800000) & 0xFF8FFFFF );
			delay_ms(100);
		}
		if ((IO1PIN & (1 << 24)) == 0) {
			printBeverage("Coca Cola", "Rs 35");
			IO0PIN = ( (IO0PIN | 0x02000000) & 0xE3FFFFFF );
			delay_ms(100);
			
			IO0PIN = ( (IO0PIN | 0x04000000) & 0xE5FFFFFF );
			delay_ms(100);
			
			IO0PIN = ( (IO0PIN | 0x08000000) & 0xE9FFFFFF );
			delay_ms(100);
			
			IO0PIN = ( (IO0PIN | 0x10000000) & 0x11FFFFFF );
			delay_ms(100);
		}
		if ((IO1PIN & (1 << 26)) == 0) {
			printBeverage("Lime Juice", "Rs 15");
			IO1PIN &= 0xFFFEFFFF;
			IO0PIN = ( (IO0PIN | 0x20000000) & 0x3FFFFFFF );
			delay_ms(100);
			
			IO0PIN = ( (IO0PIN | 0x40000000) & 0x5FFFFFFF );
			delay_ms(100);
			
			IO0PIN = ( (IO0PIN | 0x80000000) & 0x9FFFFFFF );
			delay_ms(100);
			
			IO1PIN |= 0x10000;
			IO0PIN &= 0x1FFFFFFF;
			delay_ms(100);
		}
	}
}
