#include "stm32f4xx.h"
#include "_mcpr_stm32f407.h"
#include <inttypes.h>
#include <stdlib.h>
#include "utils.h"
#include "led.h"
#include "lcd.h"
#include "timer.h"


uint8_t getButtonState(void) {
	return GPIOA->IDR & 1;
}

uint16_t secSinceReset = 0;
char time_str[6];  // "MM:SS" + null terminator

int main(void)
{
	mcpr_SetSystemCoreClock();
	LEDs_Init();
	LCD_Init();
	Timer_Init();

	uint32_t last50ms = 0;
	uint32_t last500ms = 0;

	const uint32_t lcdCol1 = hex_to_rgb565(0x36454F);
	const uint32_t lcdCol2 = hex_to_rgb565(0x51BA8E);
	const uint8_t lcdBoxX = 10;
	const uint8_t lcdBoxY = 190;
	const uint8_t lcdBoxW = 120;
	const uint8_t lcdBoxH = 50;

	LCD_ClearDisplay(lcdCol1);
	LCD_WriteString(10, 10, lcdCol2, lcdCol1, "Hello my friend!");

	LCD_DrawRectangle(lcdBoxX, lcdBoxY, lcdBoxW, lcdBoxH, lcdCol2);
	LCD_DrawRectangle(lcdBoxX + 2, lcdBoxY + 2, lcdBoxW - 4, lcdBoxH - 4, lcdCol1);
	LCD_WriteString(lcdBoxX + 10, lcdBoxY + 10, lcdCol2, lcdCol1, "ON since:");

	// green LED
	RCC->AHB1ENR |= 1<<3;
	GPIOD->MODER |= 1<<(12*2);
	GPIOD->ODR |= 1<<12;

	// user btn
	RCC->AHB1ENR |= 1;

	while (1) {
		if ((ms_Ticks - last50ms) >= 50) {
			last50ms = ms_Ticks;

			if (getButtonState()) {
				if ((ms_Ticks - last500ms) >= 500) {
					last500ms = ms_Ticks;
					GPIOD->ODR ^= 1<<12;
				}
				Timer_EnableBackLight(10);
			}
			else {
				GPIOD->ODR &= ~(1<<12);
			}

			LEDs_NextTick();

			// update counter on display
			if (secSinceReset < ms_Ticks / 1000) {
				secSinceReset = ms_Ticks / 1000;
				format_time(secSinceReset, time_str);
				LCD_WriteString(lcdBoxX + 10, lcdBoxY + 26, lcdCol2, lcdCol1, time_str);
			}

		}
	}
}
