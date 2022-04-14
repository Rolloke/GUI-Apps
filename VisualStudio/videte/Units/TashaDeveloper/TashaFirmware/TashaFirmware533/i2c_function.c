/********************************************************************/
/* I2C Funktionen													*/
/********************************************************************/
/* Autor: Christian Lohmeier										*/
/* Datum: 18.06.2003												*/
/* Firma: Videte IT	AG												*/
/********************************************************************/

#include "main.h"

void i2c_ini()
{
//Variablen Deklarierung	
int nI;

//PF0 = SCK, PF1 = SDA, PF2 = !OE 	
	*pFIO_INEN = 0x0007;		//workaround: schreiben in FIO_DIR 
	*pFIO_POLAR = 0x0000;   	//            verfälscht andere PF's
	*pFIO_EDGE = 0x0000;		//----------------------------------
	*pFIO_DIR = OUT;	
	*pFIO_FLAG_S = SCK|SDA;
	*pFIO_FLAG_C = 0x04;	
	
//EBUI Initialisieren Asynchrones Memory (Flash)
	*pEBIU_AMBCTL0	= 0x7bb07bb0; 	// Timing Control für Bank 1 und 0
	*pEBIU_AMBCTL1	= 0x7bb07bb0;	// Timing Control für Bank 2 
	*pEBIU_AMGCTL	= 0x000f;	    // Enable alle Bänke

//FlashA PortA ADC/DAC aus dem Reset	
	*pFlashA_PortA_Dir = 0x1c;
	*pFlashA_PortA_Data = 0x1c;

//Nach dem Reset warten	10ms
	for(nI=0; nI<10; nI++)
		wait(1000);	
return;
}

void i2c_start()
{	
	*pFIO_FLAG_S = SDA; 	//
	wait(3);		        //SCK-----|
	*pFIO_FLAG_S = SCK;	    //        |--
	wait(3);			    //
	*pFIO_FLAG_C = SDA;	    //SDA--|
	wait(3);			    //     |-----
    *pFIO_FLAG_C = SCK;	    //
return;
}

void i2c_stop()
{
	wait(3);               	//SCK  |-----
	*pFIO_FLAG_S = SCK;     //   --|
	wait(3);                //
	*pFIO_FLAG_S = SDA;	    //SDA     |--
	wait(3);		        //   -----|
return;
}

void i2c_putbyte(BYTE byWort)		// Sendet ein Byte über den I2C Bus 
{
	short sCountB;
	BYTE byBuff;
	for (sCountB = 7; sCountB >= 0; sCountB--)		// steuert Bitweise den I2C Bus 
	{
		*pFIO_DIR = OUT;
		wait(3);
		byBuff = byWort;			 	// sWort wird übergeben
        // sBuff um den counter schieben und LSB ausmaskieren
		byBuff = (byBuff >> sCountB) & 0x0001;
		*pFIO_FLAG_C = ~(byBuff<<1) & 0x0002; 	// wenn 0 PF1 clear
		*pFIO_FLAG_S =  (byBuff<<1) & 0x0002;	// wenn 1 PF1 set
		wait(3);
		*pFIO_FLAG_S = SCK;
		wait(3);
		*pFIO_FLAG_C = SCK;			
	}
	*pFIO_DIR = IN;
	*pFIO_INEN = 0x0002;
	wait(3);
return;
}

BYTE i2c_getbyte()			// Liest ein Byte vom I2C BUs 
{
	short sCountB;
	BYTE byBuff = 0x00;
	*pFIO_DIR = IN;
	*pFIO_INEN = 0x0002;
	wait(3);
	for (sCountB = 7; sCountB >= 0; sCountB--) 		// Empfängt bitweise vom I2C BUs
	{
		wait(3);
		*pFIO_FLAG_S = SCK;
		wait(3);
		// PF1 lesen und in sBuff an Stelle sCount
        byBuff = byBuff|(((*pFIO_FLAG_D & 0x0002)>>1)<<sCountB); 
		*pFIO_FLAG_C = SCK;
        wait(3);	
	}	
return (byBuff);
}

void i2c_nack()    	// sendet no acknowledge
{
/*  *pFIO_INEN = 0x0002; 
	*pFIO_POLAR = 0x0000;
	*pFIO_EDGE = 0x0000;
*/	*pFIO_DIR = OUT;
	*pFIO_FLAG_S = SDA;
	wait(3);
	*pFIO_FLAG_S = SCK;
	wait(3);
	*pFIO_FLAG_C = SCK;
	wait(3);
	*pFIO_FLAG_C = SDA;
return;
}

bool wait(BYTE4 nMicroSeconds)		// Warteschleife (0us.. 7,9s)
{
    BYTE4 nExpCycle = ((CCLK/1000000)*nMicroSeconds);
	if (nMicroSeconds > 7900000)
    	return(false); 
	startc();
    while (getCyclesLo() <= nExpCycle)
    	asm("nop;");
    return(true);
}
void i2c_ack()    	// sendet acknowledge
{
	*pFIO_DIR = OUT;
	*pFIO_FLAG_C = SDA;
	wait(3);
	*pFIO_FLAG_S = SCK;
	wait(3);
	*pFIO_FLAG_C = SCK;
	wait(3);
return;
}

bool i2c_gack()  	// empfängt acknowledge
{
	bool bResult;
	bResult = false;
	*pFIO_DIR = IN;
	*pFIO_INEN = 0x0002;
	wait(3);
	*pFIO_FLAG_S = SCK;
	wait(3);
	while ((*pFIO_FLAG_D & 0x0002) == 0x0002)
    {
		wait(3);
		*pFIO_FLAG_C = SCK;
    	*pFIO_DIR = OUT;
   		*pFIO_FLAG_C = SDA;
   	 	wait(3);
   		return (bResult);
   	}
  	bResult = true;
   	*pFIO_FLAG_C = SCK;
    *pFIO_DIR = OUT;
  	asm("ssync;");
return (bResult);
}

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

