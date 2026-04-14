#ifndef MCPR_LED_H
#define MCPR_LED_H

void LEDs_Init(void);
void LEDs_Output16BitWord(uint16_t data);
void LEDs_Write(uint16_t data);
void LEDs_NextTick(void);

#endif //MCPR_LED_H