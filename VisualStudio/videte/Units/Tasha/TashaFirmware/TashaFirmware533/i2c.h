/********************************************************************/
/* Autor: Christian Lohmeier										*/
/* Datum: 26.08.2004												*/
/* Firma: Videte IT	AG												*/
/********************************************************************/
#ifndef  __I2C_DEFINED
	#define __I2C_DEFINED
	

///////////////////////////////////////////////////////////////////////////////
// in file i2c_function.c
void i2c_ini(void);
void i2c_start(void);
void i2c_stop(void);

void i2c_putbyte(BYTE byWort);
BYTE i2c_getbyte(void);

void i2c_nack();
void i2c_ack();
bool i2c_gack();

bool i2c_write(BYTE bySlAdr, BYTE bySuAdr, BYTE byDataW);
bool i2c_read(BYTE bySlAdr, BYTE bySuAdr, BYTE *pbyDataR);
///////////////////////////////////////////////////////////////////////////////

#endif //__I2C_DEFINED
