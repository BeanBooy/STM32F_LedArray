/* Mikrocontroller-Programmierung Start-Projekt */
/* U. Margull, 29.2.2024 (V1.5)                 */

#include "stm32f4xx.h"
#include "_mcpr_stm32f407.h"
#include <inttypes.h>

void u_delay(const double sec)
{
	const uint32_t loopCycles = (uint32_t) (sec * 12900000);
	for (uint32_t i = 0; i < loopCycles; i++) { }
}

int main(void)
{
	mcpr_SetSystemCoreClock();
	// enable peripheral GPIOD
	RCC->AHB1ENR |= 1<<3|1;
	// enable red and green LED
	GPIOD->MODER |= 0x05000000;

	while (1) {
		if ((GPIOA->IDR & 1) != 0) {
			GPIOD->ODR ^= 1<<12;  // toggle
			u_delay(.5);
		} else {
			GPIOD->ODR &= ~(1<<12);  // off
		}
	}

}

