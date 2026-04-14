#ifndef MCPR_LCD_H
#define MCPR_LCD_H

void LCD_Output16BitWord(uint16_t data);
void LCD_WriteData(uint16_t data);
void LCD_WriteCommand(uint16_t cmd);
void LCD_WriteReg(uint16_t cmd, uint16_t data);
void LCD_Init(void);
void LCD_Reset(void);

void LCD_SetCursor(uint32_t x, uint32_t y);
void LCD_DrawPixel(uint16_t color);
void LCD_ClearDisplay(uint16_t color);

void LCD_WriteLetter(uint32_t x, uint32_t y, uint16_t color_fg, uint16_t color_bg, char letter);
void LCD_WriteString(uint32_t x, uint32_t y, uint16_t color_fg, uint16_t color_bg, const char* str);

void LCD_DrawLine(uint16_t x, uint16_t y, uint16_t length, uint16_t color);
void LCD_DrawRectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color);

uint8_t LCD_GetBackLightState(void);
void LCD_SetBackLight(uint16_t value);

// copied from the script
void LCD_Measurement(void);

#endif //MCPR_LCD_H