
#include "gpiolib.h"

int main(void)
{
  int pin, val;

  /* Enable GPIO Output Pins     */
  for (pin = 23; pin <= 23; pin++)
  { 
    if (gpio_export(pin) < 0)
      return(1);
  }
  /* Set GPIO Output directions */
  for (pin = 23; pin <= 23; pin++)
  {
    if (gpio_direction(pin, OUT) < 0)
      return(2);
  }
  /* Enable GPIO Input Pin */
//  gpio_export(17);
  /* Set GPIO Input direction */
  
//  gpio_direction(17,IN);
  for (int i=0; i<1000000; ++i)
  {
      udelay(350);
      gpio_write(23, LOW);
      udelay(350);
      gpio_write(23, HIGH);
  }
  
/*
    pin = 22;
    val = 0;
    while(val == 0)
    {
        gpio_write(pin, LOW);
        pin++;
        if (pin == 26)
        { 
            pin = 22; 
        }
        gpio_write(pin, HIGH);
        delay(500);
        val = gpio_read(17);
    }
*/
  /* Switch off */
  //gpio_write(pin, LOW);

  /* Disable GPIO pins   */
  
    for (pin = 23; pin <= 23; pin++)
    {
       if (gpio_unexport(pin) < 0)
          return(1);
    }
//    if (gpio_unexport(17) < 0)
    //return(1);
    return(0);
}

