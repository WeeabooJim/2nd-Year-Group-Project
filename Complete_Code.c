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
#define UP PB1
#define uint8 unsigned char

void I2C_Start(void);		//Start sequence
void I2C_Stop(void);		//Stop sequence
void I2C_Write(uint8_t);	//Write byte
int8_t I2C_Read_ACK(void);	//Read byte with ACK
int8_t I2C_Read_NACK(void);	//Read byte without ACK
#define Memory_WR  0b11011110	//RTC memory write addres
#define Memory_RD  0b11011111	//RTC memory Read Address

int MinutesOne_BCD_to_Decimal();		//Function to compute values from BCD RTC
int MinutesTen_BCD_To_Decimal();
int HourOne_BCD_to_Decimal();
int HourTen_BCD_to_Decimal();

char Memory_Read[17];
int8_t I2C_Read_ACK(void);	//Read byte with ACK
int8_t I2C_Read_NACK(void);	//Read byte without ACK

int N, Value1, Value2, Value3, Value4, Hours, Minutes, Hoursten, Hoursone, MinutesOne, MinutesTen;
int i, j=1, k, n=0, AlarmMinutes, AlarmHours, LeftAlarmHour, RightAlarmHour, LeftAlarmMinute, RightAlarmMinute, Alarmcount;
uint8 Minutes_BCD, Hours_BCD;
uint8_t NumDigits[10] = {					//enumeration of segment numbers
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
	DDRC = 1 << PC4 | 1 << PC5 | 1 << PC6 | 1 << PC7;		//set digit selection output
	DDRA = 1 << PA0 | 1 << PA1 | 1 << PA2 | 1 << PA3 | 1 << PA4 | 1 << PA5 | 1 << PA6;	//set segment outputs
	DDRB = 0x00;			//set button pins as input
	TWBR=17;
	TWSR=0x00;
	I2C_Start();
	I2C_Write(Memory_WR);
	I2C_Write(0x00);
	I2C_Write(0x80); //Seconds and oscillations set
	I2C_Write(0x00); // Minutes = 00
	I2C_Write(0x12); // Hours = 12			Time = 12:00
	I2C_Stop();
	_delay_us(50);
	
	while (1)
	{
		if((PINB & (1<<PB2)) == 0)		//if alarmset is pressed
		{
			_delay_ms(5);				//debounce
			if((PINB & (1<<PB2)) == 0)	//if alarmset is still pressed
			{
				if (j==1)				//first digit
				{	
					(PORTC = (PORTC | 1<<SEG1));	//turn on first segment
					PORTA = NumDigits[n];			// display n
					_delay_ms(5);
					if((PINB & (1<<PB1)) == 0)		//if up is pressed
					{
						_delay_ms(5);
						if((PINB & (1<<PB1)) == 0)
						{
							n++;					//increment number on display
						}
					}
					else if ((PINB & (1<<PB0)) == 0)	//if select is pressed
					{
						_delay_ms(5);
						if((PINB & (1<<PB0)) == 0)
						{
							_delay_ms(5);
							LeftAlarmHour = n;				//set variable Leftalarm to equal n's value
							PORTC = (PORTC & ~(1<<SEG1));	//turn off display
							n =0;							//reset n for next segment
							j++;							//move to next segment
						}
					}
					else if (n == 3)
					{
						n = 0;					//24:00 so number cant be greater than 2 first segment
					}
				}
				if (j==2)							//2nd digit
				{
					(PORTC = (PORTC | 1<<SEG2));	
					PORTA = NumDigits[n];
					_delay_ms(5);
					if((PINB & (1<<PB1)) == 0)
					{
						_delay_ms(5);
						if((PINB & (1<<PB1)) == 0)
						{
							n++;
						}
					}
					else if ((PINB & (1<<PB0)) == 0)
					{
						_delay_ms(5);
						if((PINB & (1<<PB0)) == 0)
						{
							RightAlarmHour = n;
							PORTC = (PORTC & ~(1<<SEG2));
							n=0;
							j++;
						}
					}
					else if (n == 10)
					{
						n = 0;							//number cant be greater than 9
					}
				}
				if (j==3)								//3rd digit
				{
					(PORTC = (PORTC | 1<<SEG3));
					PORTA = NumDigits[n];
					_delay_ms(5);
					if((PINB & (1<<PB1)) == 0)
					{
						_delay_ms(5);
						if((PINB & (1<<PB1)) == 0)
						{
							n++;
						}
					}
					else if ((PINB & (1<<PB0)) == 0)
					{
						_delay_ms(5);
						if((PINB & (1<<PB0)) == 0)
						{
							LeftAlarmMinute = n;
							PORTC = (PORTC & ~(1<<SEG3));
							j++;
						}
					}
					else if (n == 7)
					{
						n = 0;							//number cant be greater than 6
					}
				}
				if (j==4)							//4th digit
				{
					(PORTC = (PORTC | 1<<SEG4));
					PORTA = NumDigits[n];
					_delay_ms(5);
					if((PINB & (1<<PB1)) == 0)
					{
						_delay_ms(5);
						if((PINB & (1<<PB1)) == 0)
						{
							n++;
						}
					}
					else if ((PINB & (1<<PB0)) == 0)
					{
						_delay_ms(5);
						if((PINB & (1<<PB0)) == 0)
						{
							RightAlarmMinute = n;
							PORTC = (PORTC & ~(1<<SEG4));
							j=5;
						}
					}
					else if (n == 10)
					{
						n = 0;							//nunber cant be greater than 9
					}
				}
				if (j==5)								//display alarm
				{		
					while (Alarmcount<10)				//display alarm time for certain amount of time
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
						AlarmHours = (LeftAlarmHour * 10) + RightAlarmHour;				//compute AlarmHours
						AlarmMinutes = (LeftAlarmMinute * 10) + RightAlarmMinute;		//Compute AlarmMinutes
						Alarmcount++;
					}
				}
			}
		}
		else
		// READ RTC
		I2C_Start();
		I2C_Write(Memory_WR);
		I2C_Write(0x01);
		I2C_Start();
		I2C_Write(Memory_RD);
		Minutes_BCD=I2C_Read_ACK();					//read minutes from RTC
		Hours_BCD=I2C_Read_NACK();					//Red hours from RTC
		I2C_Stop();
		//	_delay_ms(1000);
		N = 1;
		Hoursten = HourTen_BCD_to_Decimal (Hours_BCD);
		Hoursone = HourOne_BCD_to_Decimal (Hours_BCD);					//compute decimal values from RTC BCD format
		MinutesOne = MinutesOne_BCD_to_Decimal(Minutes_BCD);
		MinutesTen = MinutesTen_BCD_To_Decimal(Minutes_BCD);
		Hours = ((Hoursten * 10) + Hoursone);
		Minutes = ((MinutesTen * 10) + MinutesOne);						//compute hours and minutes from function returns
		j=0;
			//NORMAL CLOCK OPERATION
		if(N==1)				
		{
			Value1 = Hours / 10;							//compute value for 1st segment 
			PORTA = NumDigits[Value1];							//display value before turning on so correct no, shows
			(PORTC = PORTC | 1<<SEG1);						//turn on segment
			_delay_ms(4);
			PORTC = (PORTC & ~(1<<SEG1));					//turn off segment
			N++;											//select next segment
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
		N = 1;																//return to first segment next cycle if alarm is not pressed or alarmtime does not equal time
		if ((Hours == AlarmHours) & (Minutes == AlarmMinutes))				//Alarm trigger event
		{
			N = 5;
			(PORTC = PORTC | 1<<PC3);				//turn on buzzer
			while (N==5)
			{
				PORTA = NumDigits[LeftAlarmHour];				//Following Displays Alarm Time
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
				if ((PINB & (1<<Select)) == 0)				//if select is pressed leave this alarm mode
				{
					_delay_ms(5);
					if ((PINB & (1<<Select)) == 0)				
					{
						((PORTC & (1<<PC3)) == 0)  ;      //turn off buzzer
						N=1;							//reset N to return to clock cycle
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
		Hoursten = 2;
	}
	else if (Hours_BCD & (1<<4))
	{
		Hoursten = 1;
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
