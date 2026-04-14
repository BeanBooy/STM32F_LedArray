#ifndef MCPR_TIMER_H
#define MCPR_TIMER_H

extern volatile uint32_t ms_Ticks;

void Timer_Init(void);
void Timer_EnableBackLight(uint16_t seconds);

#endif //MCPR_TIMER_H