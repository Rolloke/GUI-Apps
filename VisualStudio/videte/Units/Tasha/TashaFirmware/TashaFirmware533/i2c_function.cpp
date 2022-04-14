/********************************************************************/
/* I2C Funktionen													*/
/********************************************************************/
/* Autor: Christian Lohmeier										*/
/* Datum: 01.06.2004												*/
/* Firma: Videte IT	AG												*/
/********************************************************************/
#include "motion.h"
#include "main.h"
#include "i2c.h"

/////////////////////////////////////////////////////////////////////////////
void i2c_ini()
{
//Variablen Deklarierung	
int nI;

//PF7 = SCK, PF8 = SDA, PF6 = CS	
	*pFIO_DIR = OUT;			// SCK, SDA, CS Ausgang
	*pFIO_FLAG_C = CS;			// CS auf "0"
	WaitMicro(1);				// definierte Flanke
	*pFIO_FLAG_S = SCK|SDA|CS;	// SCK, SDA, CS auf "1"		

//Nach dem Reset warten	100ms
	for(nI=0; nI<100; nI++)
		WaitMicro(1000);	
return;
}
/////////////////////////////////////////////////////////////////////////////

void i2c_start()
{	
	*pFIO_FLAG_S = SDA; 	//
	WaitMicro(2);		    //SCK-----|
	*pFIO_FLAG_S = SCK;	    //        |--
	WaitMicro(2);			//
	*pFIO_FLAG_C = SDA;	    //SDA--|
	WaitMicro(2);			//     |-----
    *pFIO_FLAG_C = SCK;	    //
return;
}
/////////////////////////////////////////////////////////////////////////////

void i2c_stop()
{
	WaitMicro(2);           //SCK  |-----
	*pFIO_FLAG_S = SCK;     //   --|
	WaitMicro(2);           //
	*pFIO_FLAG_S = SDA;	    //SDA     |--
	WaitMicro(2);		    //   -----|
return;
}
/////////////////////////////////////////////////////////////////////////////

void i2c_putbyte(BYTE byWort)		// Sendet ein Byte über den I2C Bus 
{
	short sCountB;
	BYTE byBuff;
	for (sCountB = 7; sCountB >= 0; sCountB--)		// steuert Bitweise den I2C Bus 
	{
		*pFIO_DIR = OUT;
		WaitMicro(2);
		byBuff = byWort;			 	// byWort wird übergeben
        // byBuff um den counter schieben und LSB ausmaskieren
		byBuff = (byBuff >> sCountB) & LSB;
		*pFIO_FLAG_C = ~(byBuff<<8) & PF8; 	// wenn 0 PF8 clear
		*pFIO_FLAG_S =  (byBuff<<8) & PF8;	// wenn 1 PF8 set
		WaitMicro(2);
		*pFIO_FLAG_S = SCK;
		WaitMicro(2);
		*pFIO_FLAG_C = SCK;			
	}
	*pFIO_DIR = IN;
	*pFIO_INEN |= SDA;
	WaitMicro(2);
return;
}
/////////////////////////////////////////////////////////////////////////////

BYTE i2c_getbyte()			// Liest ein Byte vom I2C BUs 
{
	short sCountB;
	BYTE byBuff = 0x00;
	*pFIO_DIR = IN;
	*pFIO_INEN |= SDA;
	WaitMicro(2);
	for (sCountB = 7; sCountB >= 0; sCountB--) 		// Empfängt bitweise vom I2C BUs
	{
		WaitMicro(2);
		*pFIO_FLAG_S = SCK;
		WaitMicro(2);
		// PF8 lesen und in sBuff an Stelle sCount
        byBuff = byBuff| ((BYTE)((BYTE2)((*pFIO_FLAG_D & PF8)<<sCountB) >> 8)); 
		*pFIO_FLAG_C = SCK;
        WaitMicro(2);	
	}	
return (byBuff);
}
/////////////////////////////////////////////////////////////////////////////

void i2c_nack()    	// sendet no acknowledge
{
/*  *pFIO_INEN = 0x0002; 
	*pFIO_POLAR = 0x0000;
	*pFIO_EDGE = 0x0000;
*/	*pFIO_DIR = OUT;
	*pFIO_FLAG_S = SDA;
	WaitMicro(2);
	*pFIO_FLAG_S = SCK;
	WaitMicro(2);
	*pFIO_FLAG_C = SCK;
	WaitMicro(2);
	*pFIO_FLAG_C = SDA;
return;
}
/////////////////////////////////////////////////////////////////////////////

void i2c_ack()    	// sendet acknowledge
{
	*pFIO_DIR = OUT;
	*pFIO_FLAG_C = SDA;
	WaitMicro(2);
	*pFIO_FLAG_S = SCK;
	WaitMicro(2);
	*pFIO_FLAG_C = SCK;
	WaitMicro(2);
return;
}
/////////////////////////////////////////////////////////////////////////////

bool i2c_gack()  	// empfängt acknowledge
{
	bool bResult;
	bResult = false;
	*pFIO_DIR = IN;
	*pFIO_INEN |= SDA;
	WaitMicro(2);
	*pFIO_FLAG_S = SCK;
	WaitMicro(2);
	while (*pFIO_FLAG_D & SDA)
    {
		WaitMicro(2);
		*pFIO_FLAG_C = SCK;
    	*pFIO_DIR = OUT;
   		*pFIO_FLAG_C = SDA;
   	 	WaitMicro(2);
   		return (bResult);
   	}
  	bResult = true;
   	*pFIO_FLAG_C = SCK;
    *pFIO_DIR = OUT;
  	asm("ssync;");
return (bResult);
}
/////////////////////////////////////////////////////////////////////////////

bool i2c_write(BYTE bySlAdr, BYTE bySuAdr, BYTE byDataW)  	// Schreiben 
{
    i2c_start();
	i2c_putbyte(bySlAdr);	 	// Slave Adresse schreiben
	if(i2c_gack() == false)
	{
		i2c_stop();
	  	return(false);
	}
	i2c_putbyte(bySuAdr); 		// Register beschreiben
	if(i2c_gack() == false)
	{
		i2c_stop();
	  	return(false);
	}	
	i2c_putbyte(byDataW); 		 // Data schreiben
	if(i2c_gack() == false)
	{
		i2c_stop();
	  	return(false);
	}	  
    i2c_stop();
    return(true);
}
/////////////////////////////////////////////////////////////////////////////

bool i2c_read(BYTE bySlAdr, BYTE bySuAdr, BYTE *pbyDataR)  	// Lesen
{
	
    i2c_start();
	i2c_putbyte(bySlAdr & 0xFE); 	// Slave Adresse schreiben
	if(i2c_gack() == false)
	{
		i2c_stop();
	  	return(false);
	}	  
	i2c_putbyte(bySuAdr); 		// Register auswählen
	if(i2c_gack() == false)
	{
		i2c_stop();
	  	return(false);
	}	  	
	i2c_start();
	i2c_putbyte(bySlAdr | 0x01); 	// Slave Adresse lesen
	if(i2c_gack() == false)
	{
		i2c_stop();
	  	return(false);
	}	  
    	*pbyDataR = i2c_getbyte(); 	// Daten lesen und übergeben
	i2c_nack();
     i2c_stop();
    return(true);
}
/////////////////////////////////////////////////////////////////////////////

