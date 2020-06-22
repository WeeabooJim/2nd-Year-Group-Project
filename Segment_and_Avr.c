#define F_CPU 20E6
#include <avr/io.h>
#include <util/delay.h>
#include <string.h>

#define LED_Segment_Outputs 0b01111111
#define SEG1 PC4
#define SEG2 PC5
#define SEG3 PC6
#define SEG4 PC7

#define uint8 unsigned char
#define int8_t int
#define I2C_Delay _delay_us(50)
void I2C_Start(void);		//Start sequence
void I2C_Stop(void);		//Stop sequence
void I2C_Write(uint8_t);	//Write byte
int8_t I2C_Read_ACK(void);	//Read byte with ACK
int8_t I2C_Read_NACK(void);	//Read byte without ACK
#define Memory_WR  0x11011110		//RTC Write Address
#define Memory_RD  0x11011111		//RTC Read Address

int MinutesOne_BCD_to_Decimal();		//Functions to convert BCD to computable decimals
int MinutesTen_BCD_To_Decimal();
int HourOne_BCD_to_Decimal();
int HourTen_BCD_to_Decimal();

int i, j, N, Value1, Value2, Value3, Value4, Hours, Minutes, Hoursten, Hoursone, MinutesOne, MinutesTen;
char Minutes_BCD, Hours_BCD;

uint8_t NumDigits[10] = {						//enumeration of numbers
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
	TWBR=17;					//Set I2C to 400KHz from 20MHz clock
	TWSR=0x00;
	I2C_Start();
	I2C_Write(Memory_WR);	
	I2C_Write(0x00);		
	I2C_Write(0x80);			//Seconds and set oscilattions
	I2C_Write(0x00);			//Minutes = 00
	I2C_Write(0x12);			//Hours= 12			time = 12:00
	I2C_Stop();
	_delay_us(50);
	while(1)
	{
	// READ RTC
	I2C_Start();
	I2C_Write(Memory_WR);
	I2C_Write(0x01);
	I2C_Start();
	I2C_Write(Memory_RD);
	Minutes_BCD=I2C_Read_ACK();			//Read Minutes
	Hours_BCD=I2C_Read_NACK();			//Read Hours
	I2C_Stop();
	N = 1;
	HourTen_BCD_to_Decimal (Hours_BCD);			//Compute values from BCD
	HourOne_BCD_to_Decimal (Hours_BCD);
	MinutesOne_BCD_to_Decimal(Minutes_BCD);
	MinutesTen_BCD_To_Decimal(Minutes_BCD);
	Hours = ((Hoursten * 10) + Hoursone);		//Compute hours from function returns
	Minutes = ((MinutesTen * 10) + MinutesOne);
	
	if(N==1)		//First Segment
	{
		Value1 = Hours / 10;				//Compute value from hours
		PORTA = NumDigits[Value1];			//display value before turning on digit so correct number shown
		(PORTC = PORTC | 1<<SEG1);			//turn on segment
		_delay_ms(2.5);
		PORTC = (PORTC & ~(1<<SEG1));		//turn off segment
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
	N = 1;

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