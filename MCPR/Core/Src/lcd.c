#include "stm32f4xx.h"
#include "lcd.h"
#include "utils.h"
#include "font.h"

#define MASK_D (1<<14 | 1<<15 | 1<<0 | 1<<1 | 1<<8 | 1<<9 | 1<<10)
#define MASK_E (1<<7 | 1<<8 | 1<<9 | 1<<10 | 1<<11 | 1<<12 | 1<<13 | 1<<14 | 1<<15)
#define BYTES_PER_CHAR 32
#define FONT_HEIGHT 16
#define FONT_WIDTH 12

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

void LCD_WriteData(const uint16_t data)
{
    GPIOD->ODR &= ~(1<<11);  // set PD11 to low to switch from LEDs to LCD
    GPIOD->ODR |= 1<<4;  // set PD4 to high to disable RD signal
    // write sequence from data sheet
    GPIOE->ODR |= 1<<3;  // set DC signal high to select data channel
    GPIOD->ODR &= ~(1<<7);  // set CS to low (enables display interface)
    GPIOD->ODR &= ~(1<<5);  // set WR to low (enables write sequence)
    LCD_Output16BitWord(data);
    u_delay(0.000000005);  // 5ns (probably useless, since function call will already take longer...)
    GPIOD->ODR |= 1<<5;  // set WR to high (write)
    GPIOD->ODR |= 1<<7;
}

void LCD_WriteCommand(const uint16_t cmd)
{
    GPIOD->ODR &= ~(1<<11);  // set PD11 to low to switch from LEDs to LCD
    GPIOD->ODR |= 1<<4;  // set RD to high to disable read signal
    // write sequence from data sheet
    GPIOE->ODR &= ~(1<<3);  // set DC signal low to select command channel
    GPIOD->ODR &= ~(1<<7);  // set CS to low (enables display interface)
    GPIOD->ODR &= ~(1<<5);  // set WR to low (enables write sequence)
    LCD_Output16BitWord(cmd);
    u_delay(0.000000005);  // 5ns (probably useless, since function call will already take longer...)
    GPIOD->ODR |= 1<<5;  // set WR to high (write)
    GPIOD->ODR |= 1<<7;
}

void LCD_WriteReg(const uint16_t cmd, const uint16_t data)
{
    LCD_WriteCommand(cmd);
    LCD_WriteData(data);
}

void LCD_Init(void)
{
    // enable clock on GPIOD and GPIOE
    RCC->AHB1ENR |= 1<<3 | 1<<4;
    // set PE3, PD3, PD4, PD5, PD7, PD11, PD13 to output
    GPIOD->MODER |= 1<<(3*2) | 1<<(4*2) | 1<<(5*2) | 1<<(7*2) | 1<<(11*2) | 1<<(13*2);
    GPIOE->MODER |= 1<<(3*2);
    // set all the LCD data pins to output
    GPIOD->MODER |= 1<<(14*2) | 1<<(15*2) | 1<<(0*2) | 1<<(1*2) | 1<<(8*2) | 1<<(9*2) | 1<<(10*2);
    GPIOE->MODER |= 1<<(7*2) | 1<<(8*2) | 1<<(9*2) | 1<<(10*2) | 1<<(11*2) | 1<<(12*2) | 1<<(13*2) | 1<<(14*2) | 1<<(15*2);

    LCD_SetBackLight(1);

    // reset is required before init
    LCD_Reset();

    // code sequence copied from datasheet
    LCD_WriteReg(0x0010, 0x0001); /* Enter sleep mode */
    LCD_WriteReg(0x001E, 0x00B2); /* Set initial power parameters. */
    LCD_WriteReg(0x0028, 0x0006); /* Set initial power parameters. */
    LCD_WriteReg(0x0000, 0x0001); /* Start the oscillator.*/
    LCD_WriteReg(0x0001, 0x72EF); /* Set pixel format and basic display orientation */
    LCD_WriteReg(0x0002, 0x0600);
    LCD_WriteReg(0x0010, 0x0000); /* Exit sleep mode.*/
    u_delay(0.03);  // 30ms (less may also work)
    LCD_WriteReg(0x0011, 0x6870); /* Configure pixel color format and MCU interface parameters.*/
    LCD_WriteReg(0x0012, 0x0999); /* Set analog parameters */
    LCD_WriteReg(0x0026, 0x3800);
    LCD_WriteReg(0x0007, 0x0033); /* Enable the display */
    LCD_WriteReg(0x000C, 0x0005); /* Set VCIX2 voltage to 6.1V.*/
    LCD_WriteReg(0x000D, 0x000A); /* Configure Vlcd63 and VCOMl */
    LCD_WriteReg(0x000E, 0x2E00);
    LCD_WriteReg(0x0044, (240-1) << 8); /* Set the display size and ensure that the GRAM window is
    set to allow access to the full display buffer.*/
    LCD_WriteReg(0x0045, 0x0000);
    LCD_WriteReg(0x0046, 320-1);
    LCD_WriteReg(0x004E, 0x0000); /*Set cursor to 0,0 */
    LCD_WriteReg(0x004F, 0x0000);
}

void LCD_Reset(void)
{
    GPIOD->ODR |= 1<<3;
    // disable RD, WR, CS by setting them to high
    GPIOD->ODR |= 1<<4;
    GPIOD->ODR |= 1<<5;
    GPIOD->ODR |= 1<<7;
    GPIOD->ODR &= ~(1<<3);  // do reset by setting RESET pin to low
    u_delay(.000015);  // 15us min wait time
    GPIOD->ODR |= 1<<3;
}

void LCD_SetCursor(const uint32_t x, const uint32_t y)
{
    // display size: 320 x 240
    // TODO: validate x and y parameters ???? lets see how the display handles out of range values
    // x must be between 0 - 319, y must be between 0 - 239
    LCD_WriteReg(0x004E, x);  // set x
    LCD_WriteReg(0x004F, y);  // set y
}

void LCD_DrawPixel(const uint16_t color)
{
    LCD_WriteReg(0x0022, color);
    // cursor will be incremented by 1 automatically
}

void LCD_ClearDisplay(const uint16_t color)
{
    LCD_SetCursor(0, 0);
    LCD_WriteCommand(0x0022);
    for (int y = 0; y < 240 * 320; y++) {
        LCD_WriteData(color);
    }
}

void LCD_WriteLetter(const uint32_t x, const uint32_t y, const uint16_t color_fg, const uint16_t color_bg, const char letter)
{
    const unsigned char *font = console_font_12x16;
    // get the letter starting index
    const int base = letter * BYTES_PER_CHAR;

    for (int row_index = 0; row_index < FONT_HEIGHT; row_index++) {
        //get row startpoint
        const int idx = base + row_index * 2;
        // add current and following value together to get a full row: 00000000 0000xxxx
        const int bits_left = font[idx]<<8;
        const int bits_right = font[idx + 1];  // last 4 bits can actually be ignored
        const int bits = bits_left | bits_right;

        LCD_SetCursor(x, y + row_index);
        LCD_WriteCommand(0x0022);

        for (int col_index = 0; col_index < FONT_WIDTH; col_index++) {
            if (bits & 1<<(15 - col_index))
                LCD_WriteData(color_fg);
            else
                LCD_WriteData(color_bg);
        }
    }

    // place cursor behind printed letter to simplify string printing
    LCD_SetCursor(x + FONT_WIDTH, y);
}

void LCD_WriteString(const uint32_t x, const uint32_t y, const uint16_t color_fg, const uint16_t color_bg, const char* str)
{
    for (int i = 0; str[i] != '\0'; i++) {
        LCD_WriteLetter(x + (i * FONT_WIDTH), y, color_fg, color_bg, str[i]);
    }
}

void LCD_Measurement(void)
{
    // copied from the script
    uint16_t color_bg = 0xF800;
    uint16_t color_fg = 0xFFFF;
    while (1) {
        GPIOD->ODR ^= 0x1000;  // toggle Pin PD12 (green LED)
        LCD_ClearDisplay(color_bg);
        LCD_WriteString( 20, 220, color_fg, color_bg, "Hallo Welt!" );
        // change the colors
        color_fg = color_bg;
        color_bg = ~color_bg;
    }
}

void LCD_DrawRectangle(const uint16_t x, const uint16_t y, const uint16_t width, const uint16_t height, const uint16_t color)
{
    LCD_SetCursor(x, y);
    LCD_WriteCommand(0x0022);
    for (int i = 0; i < height; i++) {
        LCD_DrawLine(x, y + i, width, color);
    }
}

void LCD_DrawLine(const uint16_t x, const uint16_t y, const uint16_t length, const uint16_t color) {
    LCD_SetCursor(x, y);
    LCD_WriteCommand(0x0022);
    for (int i = 0; i < length; i++) {
        LCD_WriteData(color);
    }
}

uint8_t isBacklightOn = 0;

uint8_t LCD_GetBackLightState(void) {
    return isBacklightOn;
}

void LCD_SetBackLight(const uint16_t value) {
    if (value > 0)
        GPIOD->ODR |= 1<<13;
    else
        GPIOD->ODR &= ~(1<<13);
    isBacklightOn = value;
}