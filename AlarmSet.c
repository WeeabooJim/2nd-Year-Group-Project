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
#define BUZZER PC2
#define Alarmset PB2
#define Select PB0
#define Up PB1
#define uint8 unsigned char

void I2C_Start(void);		//Start sequence
void I2C_Stop(void);		//Stop sequence
void I2C_Write(uint8_t);	//Write byte
int8_t I2C_Read_ACK(void);	//Read byte with ACK
int8_t I2C_Read_NACK(void);	//Read byte without ACK
#define Memory_WR  0b11011110
#define Memory_RD  0b11011111

int MinutesOne_BCD_to_Decimal();
int MinutesTen_BCD_To_Decimal();		//Functions to convert BCD to Decimal
int HourOne_BCD_to_Decimal();
int HourTen_BCD_to_Decimal();

char Memory_Read[17];
int8_t I2C_Read_ACK(void);	//Read byte with ACK
int8_t I2C_Read_NACK(void);	//Read byte without ACK

int N, Value1, Value2, Value3, Value4, Hours=42, Minutes=01, Hoursten, Hoursone, MinutesOne, MinutesTen;
int i, j=1, k, n=0, AlarmMinutes, AlarmHours, LeftAlarmHour, RightAlarmHour, LeftAlarmMinute, RightAlarmMinute, Alarmcount;
uint8 Minutes_BCD, Hours_BCD;
uint8_t NumDigits[10] = {
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
	DDRC = 1 << PC4 | 1 << PC5 | 1 << PC6 | 1 << PC7 | 1<<PC3;			//Set Digit selection outputs
	DDRA = 1 << PA0 | 1 << PA1 | 1 << PA2 | 1 << PA3 | 1 << PA4 | 1 << PA5 | 1 << PA6;		//set Segment outputs
	DDRB = 0x00;
	while (1)
	{
		// ALARM SET //
		if((PINB & (1<<Alarmset)) == 0)			//Alarmset button pressed
		{
			_delay_ms(5);						//debounce
			if((PINB & (1<<Alarmset)) == 0)		//Alarmset button still pressed
			{
				if (j==1)						//first digit
				{
					(PORTC = (PORTC | 1<<SEG1));	//first digit turned on
					PORTA = NumDigits[n];			//display segments on digit
					_delay_ms(5);					
					if((PINB & (1<<Up)) == 0)		//if increment button pressed
					{
						_delay_ms(5);
						if((PINB & (1<<Up)) == 0)
						{
							n++;					//+1 to number on display
						}
					}
					else if ((PINB & (1<<Select)) == 0)  //Select button pressed
					{
						if((PINB & (1<<Select)) == 0)
						{
							_delay_ms(5);
							LeftAlarmHour = n;				//Assign number to variable LeftAlarm
							PORTC = (PORTC & ~(1<<SEG1));		//Turn off segment
							n = 0;							//reset n so next segment displays 0
							j++;							//go to next segment
						}
					}
					else if (n == 3)
					{
						n = 0;						//24:00 clock so 1st digit not greater than 2
					}
				}
				if (j==2)						//2nd segment
				{
					(PORTC = (PORTC | 1<<SEG2));
					PORTA = NumDigits[n];
					_delay_ms(5);
					if((PINB & (1<<Up)) == 0)
					{
						_delay_ms(5);
						if((PINB & (1<<Up)) == 0)
						{
							n++;
						}
					}
					else if ((PINB & (1<<Select)) == 0)
					{
						_delay_ms(5);
						if((PINB & (1<<Select)) == 0)
						{
							RightAlarmHour = n;
							PORTC = (PORTC & ~(1<<SEG2));
							n=0;
							j++;
						}
					}
					else if (n == 10)
					{
						n = 0;					//Number cant be greater than 9
					}
				}
				if (j==3)						//3rd segment
				{
					(PORTC = (PORTC | 1<<SEG3));
					PORTA = NumDigits[n];
					_delay_ms(5);
					if((PINB & (1<<Up)) == 0)
					{
						_delay_ms(5);
						if((PINB & (1<<Up)) == 0)
						{
							n++;
						}
					}
					else if ((PINB & (1<<Select)) == 0)
					{
						_delay_ms(5);
						if((PINB & (1<<Select)) == 0)
						{
							LeftAlarmMinute = n;
							PORTC = (PORTC & ~(1<<SEG3));
							j++;
						}
					}
					else if (n == 7)
					{
						n = 0;					//Number cant be greater than 6
					}
				}
				if (j==4)						// 4th segment
				{
					(PORTC = (PORTC | 1<<SEG4));
					PORTA = NumDigits[n];
					_delay_ms(5);
					if((PINB & (1<<Up)) == 0)
					{
						_delay_ms(5);
						if((PINB & (1<<Up)) == 0)
						{
							n++;
						}
					}
					else if ((PINB & (1<<Select)) == 0)
					{
						_delay_ms(5);
						if((PINB & (1<<Select)) == 0)
						{
							RightAlarmMinute = n;
							PORTC = (PORTC & ~(1<<SEG4));
							j=5;
						}
					}
					else if (n == 10)
					{
						n = 0;					//number cant be greater than 9
					}
				}
				if (j==5)					 //show alarm set number
				{
					while (Alarmcount<10)	//display alarm time for certain amount of time
					{
						PORTA = NumDigits[LeftAlarmHour];
						(PORTC = PORTC | 1<<SEG1);
						_delay_ms(2.5);
						PORTC = (PORTC & ~(1<<SEG1));
						PORTA = NumDigits[RightAlarmHour];
						(PORTC = PORTC | 1<<SEG2);
						_delay_ms(2.5);
						PORTC = (PORTC & ~(1<<SEG2));
						PORTA = NumDigits[LeftAlarmMinute];
						(PORTC = PORTC | 1<<SEG3);
						_delay_ms(2.5);
						PORTC = (PORTC & ~(1<<SEG3));
						PORTA = NumDigits[RightAlarmMinute];
						(PORTC = PORTC | 1<<SEG4);
						_delay_ms(2.5);
						PORTC = (PORTC & ~(1<<SEG4));
						AlarmHours = (LeftAlarmHour * 10) + RightAlarmHour;				//set alarmhours
						AlarmMinutes = (LeftAlarmMinute * 10) + RightAlarmMinute;		//set alarmminutes
						Alarmcount++;
					}
				}
			}
		}
		asm("nop");
	}
}
/******************************/

void I2C_Write(uint8_t number)
{
	TWDR=number;
	TWCR=1<<TWINT | 1<< TWEN;
	while(!(TWCR & 1<<TWINT))
	{
	}
}
/***********************************************************/
void I2C_Start(void)
{
	TWCR=1<<TWINT | 1<< TWEN | 1<<TWSTA;
	while(!(TWCR & 1<<TWINT))
	{
	}
}
/***********************************************************/
void I2C_Stop(void)
{
	TWCR=1<<TWINT | 1<< TWEN | 1<<TWSTO;
}
/***********************************************************/
int8_t I2C_Read_NACK(void)
{
	TWCR=1<<TWINT | 1<< TWEN;
	while(!(TWCR & 1<<TWINT))
	{
	}
	return(TWDR);
}
/***********************************************************/
int8_t I2C_Read_ACK(void)
{
	TWCR=1<<TWINT | 1<< TWEN | 1<<TWEA;
	while(!(TWCR & 1<<TWINT))
	{
	}
	return(TWDR);
}
/******************************/
int HourTen_BCD_to_Decimal (void)
{
	if( Hours_BCD &(1<<5))
	{
		Hoursten = 20;
	}
	else if (Hours_BCD & (1<<4))
	{
		Hoursten = 10;
	}
	else
	{
		Hoursten = 0;
	}
	return (Hoursten);
}
/****************************/
int HourOne_BCD_to_Decimal (void)
{
	if (Hours_BCD & (1<<3))
	{
		Hoursone = 8;					//8 = Bit3
		if (Hours_BCD & (1<<0))
		{
			Hoursone = 9;				//9 = Bit3 & Bit0
		}
	}
	else if (Hours_BCD & (1<<2))
	{
		Hoursone = 4;					//4 = Bit2
		if (Hours_BCD & (1<<1))
		{
			Hoursone = 6;				//6 = Bit2 & Bit1
			if (Hours_BCD & (1<<0))
			{
				Hoursone = 7;			// 7 = Bit2 & Bit1 & Bit0
			}
		}
		if (Hours_BCD & (1<<0))
		{
			Hoursone = 5;				// 5 = Bit0 & bit2
		}
	} 
	else if (Hours_BCD & (1<<1))
	{
		Hoursone = 2;					//2 = Bit1
		if (Hours_BCD & (1<<0))
		{
			Hoursone = 3;				//3 = bit0 * bit1
		}
	}
	else if (Hours_BCD & (1<<0))
	{
		Hoursone = 1;				//1 = bit0
	}
	else
	{
		Hoursone = 0;					//0 = no bits
	}
	return (Hoursone);
}
/**************************************/
int MinutesOne_BCD_to_Decimal (void)
{
	if (Minutes_BCD & (1<<3))
	{
		MinutesOne = 8;					//8 = Bit3
		if (Minutes_BCD & (1<<0))
		{
			MinutesOne = 9;				//9 = Bit3 & Bit0
		}
	}
	else if (Minutes_BCD & (1<<2))
	{
		MinutesOne = 4;					//4 = Bit2
		if (Minutes_BCD & (1<<1))
		{
			MinutesOne = 6;				//6 = Bit2 & Bit1
			if (Minutes_BCD & (1<<0))
			{
				MinutesOne = 7;			// 7 = Bit2 & Bit1 & Bit0
			}
		}
		else if (Minutes_BCD & (1<<0))
		{
			MinutesOne = 5;				// 5 = Bit0 & bit2
		}
	}
	else if (Minutes_BCD & (1<<1))
	{
		MinutesOne = 2;					//2 = Bit1
		if (Minutes_BCD & (1<<0))
		{
			MinutesOne = 3;				//3 = bit0 * bit1
		}
	}
	else if (Minutes_BCD & (1<<0))
	{
		MinutesOne = 1;				//1 = bit0
	}
	else
	{
		MinutesOne = 0;					//0 = no bits
	}
	return (MinutesOne);
}
/**************************************/
int MinutesTen_BCD_To_Decimal (void)
{
	if (Minutes_BCD & (1<<6))
	{
		MinutesTen = 4;
		if (Minutes_BCD & (1<<5))
		{
			MinutesTen = 6;
		}
		else if (Minutes_BCD & (1<<4))
		{
			MinutesTen = 5;
		}
	}
	else if (Minutes_BCD & (1<<5))
	{
		MinutesTen = 2;
		if (Minutes_BCD & (1<<4))
		{
			MinutesTen = 3;
		}
	}
	else if (Minutes_BCD & (1<<4))
	{
		MinutesTen = 1;
	}
	else
	{
		MinutesTen = 0;
	}
	return (MinutesTen);
	
}

