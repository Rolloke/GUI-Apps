
#include "gpioclass.h"

int main(void)
{
	CGPIO fgpio(23, CGPIO::Out);

	for (int i=0; i<10000000; ++i)
	{
		fgpio.value(1);
		udelay(1);
		fgpio.value(0);
		udelay(1);
	}
	return 0;
}
