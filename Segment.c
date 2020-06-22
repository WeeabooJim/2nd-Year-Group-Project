#define F_CPU 20E6
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <string.h>

#define LED_Segment_Outputs 0b01111111
#define SEG1 PC4
#define SEG2 PC5
#define SEG3 PC6
#define SEG4 PC7

int N, Value1, Value2, Value3, Value4, Hours, Minutes, Hoursten, Hoursone, MinutesOne, MinutesTen;
int i, j=1, k, n=0;
uint8_t NumDigits[10] = {				//Enumeration of segment numbers
/*0*/ 1<<PA0 | 1<<PA1 | 1<<PA2 | 1<<PA3 | 1<<PA4 | 1<<PA5,
/*1*/ 1<<PA4 | 1<<PA5,
/*2*/ 1<<PA0 | 1<<PA1 | 1<<PA6 | 1<<PA4 | 1<<PA3,
/*3*/ 1<<PA0 | 1<<PA1 | 1<<PA6 | 1<<PA2 | 1<<PA3,
/*4*/ 1<<PA5 | 1<<PA6 | 1<<PA1 | 1<<PA2,
/*5*/ 1<<PA0 | 1<<PA5 | 1<<PA6 | 1<<PA2 | 1<<PA3,
/*6*/ 1<<PA5 | 1<<PA4 | 1<<PA3 | 1<<PA2 | 1<<PA6,
/*7*/ 1<<PA0 | 1<<PA1 | 1<<PA2,
/*8*/ 1<<PA0 | 1<<PA1 | 1<<PA2 | 1<<PA3 | 1<<PA4 | 1<<PA5 | 1<<PA6,
/*9*/ 1<<PA0 | 1<<PA1 | 1<<PA2 | 1<<PA5 | 1<<PA6
};


int main(void)
{
	while (1)
	{
		Hours = 15;			//arbritrary numbers to show on display
		Minutes = 40;
		if(N==1)			//first segments
		{
			Value1 = Hours / 10;		//compute value for 1st segment
			PORTA = NumDigits[Value1];		//display value before segment on so right number is shown
			(PORTC = PORTC | 1<<SEG1);		//turn on segment
			_delay_ms(2.5);					//delay (2.5x4 = 10) = 100x a second
			PORTC = (PORTC & ~(1<<SEG1));	//turn segment off
			N++;
		}
		if(N==2)
		{
			Value2 = Hours % 10;
			PORTA = NumDigits[Value2];
			(PORTC = PORTC | 1<<SEG2);
			_delay_ms(2.5);
			PORTC = (PORTC & ~(1<<SEG2));
			N++;
		}
		if(N==3)
		{
			Value3 = Minutes / 10;
			PORTA = NumDigits[Value3];
			(PORTC = PORTC | 1<<SEG3);
			_delay_ms(2.5);
			PORTC = (PORTC & ~(1<<SEG3));
			N++;
		}
		if(N==4)
		{
			Value4 = Minutes % 10;
			PORTA = NumDigits[Value4];
			(PORTC = PORTC | 1<<SEG4);
			_delay_ms(2.5);
			PORTC = (PORTC & ~(1<<SEG4));
		}
		N = 1;		//restart 
	}
asm("nop");
}

