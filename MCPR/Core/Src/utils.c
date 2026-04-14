#include "stm32f4xx.h"
#include "utils.h"

#include <stdio.h>

void u_delay(const double sec)
{
    const uint32_t loopCycles = (uint32_t) (sec * 12900000);
    for (uint32_t i = 0; i < loopCycles; i++) { }
}

uint16_t rgb888_to_rgb565(const uint8_t r, const uint8_t g, const uint8_t b)
{
    const uint16_t shortened_r = r>>3;  // 5 bits
    const uint16_t shortened_g = g>>2;  // 6 bits
    const uint16_t shortened_b = b>>3;  // 5 bits
    return shortened_r<<11 | shortened_g<<5 | shortened_b;
}

uint16_t hex_to_rgb565(const uint32_t hex)
{
    return rgb888_to_rgb565(hex>>16 & 0xFF, hex>>8 & 0xFF, hex & 0xFF);
}

void format_time(const int total_seconds, char *output) {
    const int minutes = total_seconds / 60;
    const int seconds = total_seconds - (minutes * 60);
    sprintf(output, "%02d:%02d", minutes, seconds);
}