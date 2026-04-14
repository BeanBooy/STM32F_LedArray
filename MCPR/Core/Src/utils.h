#ifndef MCPR_UTILS_H
#define MCPR_UTILS_H

void u_delay(double sec);
uint16_t rgb888_to_rgb565(uint8_t r, uint8_t g, uint8_t b);
uint16_t hex_to_rgb565(uint32_t hex);
void format_time(int total_seconds, char *output);

#endif //MCPR_UTILS_H