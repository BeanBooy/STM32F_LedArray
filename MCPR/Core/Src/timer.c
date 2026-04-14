#include "stm32f407xx.h"
#include "timer.h"
#include "lcd.h"

volatile uint32_t ms_Ticks = 0;

int32_t backlightTicksLeft = -1;

void Timer_Init(void) {
    RCC->APB1ENR |= 1<<5;  // enable TIM7 peripheral
    TIM7->PSC = 83;  // set prescaler to 84 (MC runs at 84MHz) -> tick at 1MHz/1us
    TIM7->ARR = 999;  // set auto-reload to 1000 -> restart timer every 1ms/1kHz
    TIM7->DIER |= 1;  // enable interrupt
    NVIC_EnableIRQ(TIM7_IRQn);
    TIM7->CR1 |= 1;  // turn timer on
}

void TIM7_IRQHandler(void) {
    if (TIM7->SR & 1) {
        TIM7->SR &= ~1;
    }
    ms_Ticks++;
    // handle the LCD backlight
    if (backlightTicksLeft == 0)
        LCD_SetBackLight(0);
    if (backlightTicksLeft > -1)
        backlightTicksLeft--;
}

void Timer_EnableBackLight(const uint16_t seconds) {
    backlightTicksLeft = seconds * 1000;
    if (LCD_GetBackLightState() == 0)
        LCD_SetBackLight(1);
}