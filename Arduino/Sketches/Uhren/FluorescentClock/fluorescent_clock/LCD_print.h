
#define LCD_PRINTER 1

#ifdef LCD_PRINTER

  #include <LiquidCrystal.h>
  #define LCD_XDIM 16
  #define LCD_YDIM 4
  #define LEFT   -1
  #define CENTER -2
  #define RIGHT  -3
  
  #define LCD_BEGIN           LCD.begin(LCD_XDIM, LCD_YDIM);
  #define LCD_SETCURSOR(X,Y)  LCD.setCursor(X, Y);
  #define LCD_PRINT(P)        LCD.print(P);
  #define LCD_PRINT_AT(X,Y,P)  LCD.setCursor(X, Y); LCD.print(P); 
  #define LCD_PRINT_LINE(X,Y,P,L) { \
   int fX = X; \
   if (fX == CENTER) fX = (LCD_XDIM - L) / 2; \
   if (fX == LEFT)   fX = 0; \
   if (fX == RIGHT)  fX = LCD_XDIM - L; \
   LCD.setCursor(0, Y); \
   for (int i=0; i<fX; ++i) LCD.print(" "); \
   LCD.print(P); \
   int fAppend = LCD_XDIM - L - fX; \
   for (int i=0; i<fAppend; ++i) LCD.print(" "); \
  }

  
  extern LiquidCrystal LCD;
  
#else

  #define LCD_BEGIN //
  #define LCD_SETCURSOR //
  #define LCD_PRINT     //
  #define LCD_PRINT_AT  //
  
#endif

// todo! this must be defined once in .ino file
//#ifdef LCD_PRINT
//  LiquidCrystal LCD(rs, [rw,] enable, d0, d1, d2, d3 [, d4, d5, d6, d7]);
//#endif
//  rs    : LOW: command.  HIGH: character.
//  [rw]  : optional flag to read from LCD, LOW: write to LCD.  HIGH: read from LCD.
//  enable: activated by a HIGH pulse.
//  d0 - d3: data 4 bit
//  [d4- d7]: optional data for 8 bit
//  Use two extra PWM pins for:
//   - contrast: 0-1 V
//   - LED Backlight: 

