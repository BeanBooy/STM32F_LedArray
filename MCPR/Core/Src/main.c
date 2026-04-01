#include "stm32f4xx.h"
#include "_mcpr_stm32f407.h"
#include <inttypes.h>

#define MASK_D (1<<14 | 1<<15 | 1<<0 | 1<<1 | 1<<8 | 1<<9 | 1<<10)
#define MASK_E (1<<7 | 1<<8 | 1<<9 | 1<<10 | 1<<11 | 1<<12 | 1<<13 | 1<<14 | 1<<15)


void u_delay(const double sec)
{
	const uint32_t loopCycles = (uint32_t) (sec * 12900000);
	for (uint32_t i = 0; i < loopCycles; i++) { }
}

void LEDs_InitPorts(void)
{
	// enable clock on GPIOD and GPIOE
	RCC->AHB1ENR |= 1<<3 | 1<<4;
	// set PD5, PD7 and PD11 to output
	GPIOD->MODER |=  1<<(5*2) | 1<<(7*2) | 1<<(11*2);
	// set all the display data pins to output
	GPIOD->MODER |= 1<<(14*2) | 1<<(15*2) | 1<<(0*2) | 1<<(1*2) | 1<<(8*2) | 1<<(9*2) | 1<<(10*2);
	GPIOE->MODER |= 1<<(7*2) | 1<<(8*2) | 1<<(9*2) | 1<<(10*2) | 1<<(11*2) | 1<<(12*2) | 1<<(13*2) | 1<<(14*2) | 1<<(15*2);
	// set PD5, PD7 and PD11 to high
	GPIOD->ODR |=  1<<5 | 1<<7 | 1<<11;
}

void LCD_Output16BitWord(const uint16_t data)
{
	const uint16_t odr_d =
		(data>>0 & 1)<<14 |
		(data>>1 & 1)<<15 |
		(data>>2 & 1)<<0 |
		(data>>3 & 1)<<1 |
		(data>>13 & 1)<<8 |
		(data>>14 & 1)<<9 |
		(data>>15 & 1)<<10;

	const uint16_t odr_e =
		(data>>4 & 1)<<7 |
		(data>>5 & 1)<<8 |
		(data>>6 & 1)<<9 |
		(data>>7 & 1)<<10 |
		(data>>8 & 1)<<11 |
		(data>>9 & 1)<<12 |
		(data>>10 & 1)<<13 |
		(data>>11 & 1)<<14 |
		(data>>12 & 1)<<15;

	// mask the required bits
	GPIOD->ODR = (GPIOD->ODR & ~MASK_D) | odr_d;
	GPIOE->ODR = (GPIOE->ODR & ~MASK_E) | odr_e;
}

void LEDs_Write(const uint16_t data)
{
	//LCD_Output16BitWord(data);
	GPIOD->ODR |=  1<<11;
	GPIOD->ODR &=  ~(1<<7);
	GPIOD->ODR &=  ~(1<<5);
	LCD_Output16BitWord(data);
	GPIOD->ODR |=  1<<5;
	GPIOD->ODR |=  1<<7;
	GPIOD->ODR &=  ~(1<<11);
}

int main(void)
{
	mcpr_SetSystemCoreClock();
	LEDs_InitPorts();

	uint16_t data = 1;
	uint16_t counter = 0;

	while (1) {
		counter++;
		// invert bits after complete cycle
		if (counter % 16 == 0 && counter != 0) data = ~data;
		// get overflow bit
		int const overflow = 1<<15 & data;
		// push data by one
		data = data<<1;
		// insert overflow bit
		data = data + (overflow > 0);
		LEDs_Write(data);
		u_delay(.05);
	}

}
