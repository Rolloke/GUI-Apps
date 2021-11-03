/**************************************************************************/
/*                                                                        */
/*                                 FFTRANS2                               */
/*                auswerten der RÅckgabedatei von CALGRAPH                */
/*               Nur EGA- und VGA-Karten werden unterstÅtzt.              */
/*                                                                        */
/*               (C) by ELAC Technische Software, Kiel  1997              */
/*                      Ver. 1.2       Stand 19.02.1997                   */
/*                                                                        */
/*                    Compile in COMPACT memory MODEL !!                  */
/*                                                                        */
/*                Compile with no Floating Point Emulation                */
/*                                                                        */
/*                                           programmed by Oliver Wesnigk */
/**************************************************************************/

#pragma options -f287

#ifndef __COMPACT__
#error Compile in COMPACT memory MODEL
#else

#include <stdio.h>
#include <process.h>

/***************************************************************************/
/*                                                                         */
/*                          Struktur Deklerationen                         */
/*                                                                         */
/***************************************************************************/

								     // Daten aus der Konfigurationsdatei CALE.CFG

struct cfgdata
{
   double untere,
	  frequenzstep,
	  unterefft,
	  timestep;
   float  zoom_xmax[3],                                              // Zoomwerte fÅr die Windows
	  zoom_xmin[3],                                              // 0 = Timedomain, 1= RMS, 2= Freq. Domain
	  zoom_ymax[3],
	  zoom_ymin[3];
   char   mlssa_pfad[80];					     // 11.2.97 Neu fÅr PARAMEXP
};


int     i,k;
int     error,windows;
cfgdata Cale={5.0,9.0,5.0,1.0};                                      // Initdaten fals cale.cfg zerstîrt
FILE    *fp;

/***************************************************************************/
/*                                                                         */
/*                               Hauptprogramm                             */
/*                                                                         */
/***************************************************************************/


void main()
{
   if((fp=fopen("CALE.CFG","rb"))!=NULL) fread(&Cale,sizeof(Cale),1,fp);
   fclose(fp);

								     // RÅckgabedatei von CALGRAPH auswerten

   if((fp=fopen("RCALGRAP.UEB","rt"))!=NULL)
   {
      fscanf(fp,"%d",&error);
      fscanf(fp,"%d",&windows);

      k=0;

      if (windows==2) k=2;
      if (windows==3) k=1;

      if (k!=0)                                                      // gÅltige Windowzahl dann lese Zoomwerte
      {
	 for (i=0;i<3;i+=k)
	 {
	    fscanf(fp,"%f",&Cale.zoom_xmax[i]);
	    fscanf(fp,"%f",&Cale.zoom_xmin[i]);
	    fscanf(fp,"%f",&Cale.zoom_ymax[i]);
	    fscanf(fp,"%f",&Cale.zoom_ymin[i]);
	 }

								     // aendere cale.cfg damit die Zoomwerte von Calegraph erhalten bleiben

	 fclose(fp);

	 if((fp=fopen("CALE.CFG","wb"))!=NULL) fwrite(&Cale,sizeof(Cale),1,fp);
      }

      fclose(fp);

      remove("RCALGRAP.UEB");
   }
   else
      remove("RCALWAFA.UEB");                                        // Kann ja auch von CALWAFAL kommen


								     // Programm beenden -> RÅcksprung zum aufrufendem Modul Åber CALE
   exit(0);                                                          // eingefÅgt 10.10.96 damit definierted Endecode ( wichtig fÅr systcalc !)
}
#endif
