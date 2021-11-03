/**************************************************************************/
/*                                                                        */
/*                                 FFTRANS                                */
/*                       Aufruf des Modules FFTRANS1                      */
/*                     Anschliesender Sprung in Modul FFTRANS2            */
/*                                                                        */
/*               (C) by ELAC Technische Software, Kiel  1994              */
/*                      Ver. 1.0       Stand 10.10.1996                   */
/*                                                                        */
/*                    Compile in COMPACT memory MODEL !!                  */
/*                                                                        */
/*                Compile with no Floating Point Emulation                */
/*                                                                        */
/*                                           programmed by Oliver Wesnigk */
/**************************************************************************/

#pragma option -f287

#ifndef __COMPACT__
#error Compile in COMPACT memory MODEL
#else

extern unsigned _stklen = 128;                                       // Minimalgrî·e fÅr den STACK

#include <process.h>

/***************************************************************************/
/*                                                                         */
/*                               Hauptprogramm                             */
/*                                                                         */
/***************************************************************************/


void main()
{
   // Rufe Modul zum Berechnen der Antwortfunktionen auf
   // wenn aufruf erfolgreich FFTRANS2 aufrufen, welches die RÅckgabedatei
   // von CALGRAPH auswertet oder die von CALWAFA lîscht !

   if(spawnl(P_WAIT,"FFTRANS1.EXE",0l)!=-1)
      spawnl(P_OVERLAY,"FFTRANS2.exe",0l);
}
#endif
