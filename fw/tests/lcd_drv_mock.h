/* LCD driver written by Peter "danni" Dannegger: */
/* http://www.avrfreaks.net/index.php?name=PNphpBB2&file=viewtopic&t=102296 */

#include "../lcd_drv.h"

#define NUM_LCD_LINES 	2
#define NUM_LCD_COLS   16

char LCD_CONTENTS[ NUM_LCD_LINES * NUM_LCD_COLS ];
int lcdPos = 0;



void lcd_putchar( uint8_t d ) 
{
  LCD_CONTENTS[lcdPos++] = d;
}


void lcd_puts( void *s )
{
  uint8_t* s1;
  for(s1 = s; *s1; s1++ )
    lcd_putchar( *s1 );
}


void lcd_blank( uint8_t len )                   // blank n digits
{
  memset(LCD_CONTENTS, 0, NUM_LCD_LINES * NUM_LCD_COLS);
  lcdPos = 0;
}

