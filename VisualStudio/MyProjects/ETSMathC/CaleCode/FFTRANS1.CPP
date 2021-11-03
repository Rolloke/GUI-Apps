/**************************************************************************/
/*                                                                        */
/*                                 FFTRANS1                               */
/*              Berechne verschiedene Signalantworten bei gegebener       */
/*                           öbertragungsfunktion                         */
/*               Nur EGA- und VGA-Karten werden unterstÅtzt.              */
/*                                                                        */
/*            Dieses Modul mu· mit dem Modul FFTMODUL gelinkt werden!     */
/*                                                                        */
/*                                                                        */
/*                                                                        */
/*               (C) by ELAC Technische Software, Kiel  1997              */
/*                      Ver. 1.1 Rel.1.1    Stand 19.03.1997              */
/*                                                                        */
/*                    Compile in  HUGE  memory MODEL !!                   */
/*                                                                        */
/*                Compile with no Floating Point Emulation                */
/*                                                                        */
/*                                           programmed by Oliver Wesnigk */
/**************************************************************************/

#pragma options -f287

#ifndef __HUGE__
#error Compile in HUGE memory MODEL
#else

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <conio.h>
#include <alloc.h>
#include <string.h>
#include <process.h>
#include <errno.h>
#include <dos.h>

#include "sanduhr.h"


#define DIRAC       1
#define HEAVISIED   2
#define SINEBURST   3
#define USERDEFINED 4
#define WATERFALL   5


#define FILEOPENERROR 1
#define OUTOFMEMORYERROR 5

#define FREQUENCYTOTIME 1
#define TIMETOFREQUENCY 0

#define ESC 27

#define VOIDZ (void far* far *)

/***************************************************************************/
/*                                                                         */
/*                          Struktur Deklarationen                         */
/*                                                                         */
/***************************************************************************/

								     // Daten der Konfigurationsdatei CALE.CFG

struct cfgdata
{
   double untere,                                                    // untere Grenzfrequenz fÅr die Berechnung von öbertragungsfunktionen
	  frequenzstep,                                              // StÅtzpunktweite entweder 9.0 oder 13.70
								     // FORMEL (f = untere*pow(2,s/frequenzstep)
	  unterefft,                                                 // untere Grenzfrequenz fÅr FFT-Berechnungen
	  timestep;                                                  // AbstÑnde zwischen den einzelnen Zeitfenstern fÅr Wasserfalldiagram
   float  zoom_xmax[3],                                              // Zoomwerte fÅr die Fenster von CALGRAPH bei FFT-Ergebnissen
	  zoom_xmin[3],                                              // 0 = Timedomain, 1= RMS, 2= Freq. Domain
	  zoom_ymax[3],
	  zoom_ymin[3];
};								     // Die Daten der CALE.CFG-Datei sollten von Rechenprogrammen nur
								     // gelesen werden, énderungen Åbernimmt das MODUL OPTIONS. Ein
								     // Aufruf sollte von jedem Rechenprogramm dafÅr vorgesehen werden.

								     // Daten fÅr die öbergabedatei FFTRANS.UEB (BinÑrdatei)

struct inputdata
{
   int   funktionsmode,                                              // Art der Antwortfunktion die berechnet werden soll
								     // 1=Dirac,2=Sprung,3=Sinusburst,4=User,5=Wasserfall
	 sinusperioden;                                              // Daten fÅr den Sinusburst
   float sinusfrequenz,
	 sinusamplitude;
   int   xstart,                                                     // X-Startposition fÅr den Proceedbalken
	 ystart,                                                     // Y-Startposition fÅr den Proceedbalken
	 hoehe,					                     // Hîhe des Balkens
	 laenge;						     // GesamtlÑnge des Balken bei 100 %
   char  bezeich[20];
   char  name[14];                                                   // Name der Bechnungsdatei (Netzwerk,Woofersystem...)
   char  kommentar[24];	                                     // Kommentar diese Datei
   char  artkommentar[20];                                             // letzte Hardcopykommentarzeile fÅr CALGRAPH
};
								     // Hiernach folgen 4096 StÅtzpunkte Realteil der öbertragungsfunktion
								     // und 4096 StÅtzpunkte des ImaginÑrteils


struct punkt
{
   float x,y;
};


								     // öbergabedateistruktur fÅr das Modul CALGRAPH

struct calgraph_init
{
   int   gdriver,
	 gmode,
	 windows,
	 sort;
   char  hardcopytext[8][20];
};

struct calgraph_window
{
   int   rasterfarbe,
	 achsenfarbe,
	 anzahl,
	 kurvenart,
	 kurventeilung;
   char  xtext[8],
	 ytext[8];
   float maxx,
	 minx,
	 maxy,
	 miny,
	 default_maxx,
	 default_minx,
	 default_maxy,
	 default_miny;
};

struct calgraph_kurve
{
   int   kurvenfarbe;
   char  kurventext[20];
   int   anzahlwerte;
};

/***************************************************************************/
/*                                                                         */
/*                            Globale Variablen                            */
/*                                                                         */
/***************************************************************************/

int           gmode,gdriver;                                         // Graphikmode und Treiber fÅr CALGRAPH

float         Unteregrenzfrequenz;
float         Minimalerzeitschritt;
float         Inputfunktion_gleich;                                  // GleichstromÅbertragungswert der öbertragungsfunktion
float         Inputfunktion_re[4096];                                // Real und ImaginÑrtteil der öbertragungsfunktion
float         Inputfunktion_im[4096];

inputdata     F;						     // Daten von der öbergabedatei
cfgdata       Cale;						     // Daten aus der Konfigurationsdatei

float far *   Inputsignal=NULL;				             // Zeitbereichsdaten des Eingangssignals
float far *   RMS=NULL;					             // Vektor fÅr RMS-Werte
float far *   Spektruminput=NULL;				     // Speicher fÅr die Spektren
float far *   Spektrumoutput=NULL;

int   Proceed_xstart;                                                // die Zeichenposition des Proceedbalkens
int   Proceed_ystart;
int   Proceed_hoehe;
int   Proceed_farbe=YELLOW;					     // Farbe des Proceedbalkens

long  Proceed_max;                                                   // Maximal druchzufÅhrende Rechenoperationen
int   Proceed_aktuell; 						     // ZÑhler fÅr Balkenzeichenposition
long  Proceed_done;    						     // ZÑhler der DurchgefÅhrten Rechenoperationen

float Max_input_spek;						     // Maximalamplitude des Eingangsspektrums

int   Pointsw;
int   Stuetze[156];
int   Timestep;							     // Nur fÅr Wasserfalldiagram
int   Keinefaltung=0;						     // Flag ob gefaltet werden soll
								     // (Bei Wasserfalldiagram keine Faltung)


/***************************************************************************/
/*                                                                         */
/*                            Funktions Prototypen                         */
/*                                                                         */
/***************************************************************************/


void      near Lese_config();
void      near Lese_ubergabedatei();
void      near Userdefinedfunction();
void      near Falte_im_freqdomain();
double    near Berechne_RMS(float w[]);
void      near Wasserfallberechnen();
void      near Calgraph(FILE **fp);
void      near Initcalgraph(FILE **fp,int windows,int sort);
void      near progrende(int error);
void      near Bestimmepoly(punkt *p0,punkt *p1,double m0,double m1,double coe[]);

void      near pascal Initproceed(long max);
void      far  pascal Proceed();
float     near pascal DB(double wert);
int       near pascal check_getch();
void      near pascal Speicherfreigeben(void far * far *block);


/***************************************************************************/
/*                                                                         */
/*                    Deklarationen aus dem Modul FFTMODUL                 */
/*                                                                         */
/***************************************************************************/

								     // extern aus dem Modul FFTMODUL
extern float far Fourier[];                                          // Array der Fourierkoeffizienten oder der Zeitfunktion
extern int   far Callproceed;
extern void  far RFFT(int mode);				     // FFT-ausfÅhren
extern int   far tau;


/***************************************************************************/
/*                                               			   */
/*                            FFTRANS1 HAUTPROGRAMM                        */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/*Aufgabe:                                       			   */
/*                                               			   */
/*Verschiedene                                                             */
/*                                               			   */
/*-------------------------------------------------------------------------*/
/*Parameter:                                     			   */
/*                                               			   */
/*keine                                               			   */
/*                                               			   */
/***************************************************************************/

void main()
{
   double mehr,phase,divisor,amplitude;
   double tsin,tp,timp,tp_tsin;
   int    i,k,unstetig,dauer,windowanzahl;

   float  maxy,miny,max_rms;
   FILE   *fp;

   calgraph_window calewin;
   calgraph_kurve  calekurve;

   Proceed_done = NULL;
   Callproceed  = 1;


   if((Inputsignal   = (float far *) farcalloc(4096,4))==NULL) progrende(OUTOFMEMORYERROR);
   if((RMS           = (float far *) farcalloc(4096,4))==NULL) progrende(OUTOFMEMORYERROR);
   if((Spektruminput = (float far *) farcalloc(4096,4))==NULL) progrende(OUTOFMEMORYERROR);
   if((Spektrumoutput= (float far *) farcalloc(4096,4))==NULL) progrende(OUTOFMEMORYERROR);

								     // lesen der Konfigurationsdatei

   Lese_config();

								     // bereche StÅtzpunkteabstand im Zeitbereich

   Minimalerzeitschritt = 1.0 / (8192*Unteregrenzfrequenz);

								     // lese der öbergabedatei

   Lese_ubergabedatei();


   UHR_start(Proceed_xstart-40,Proceed_ystart-4-2*(gmode==1));


   switch(F.funktionsmode)
   {
      case WATERFALL  : Callproceed=0;
			Keinefaltung=1;
			Initproceed(200l);
			break;

      case DIRAC      :                                              // NÑherungsfunktion des Dirac-Impulses
			Initproceed(53086l);
			windowanzahl = 2;

			Fourier[0]     = Unteregrenzfrequenz;
			Inputsignal[0] = 4096*Unteregrenzfrequenz;

			mehr  = Unteregrenzfrequenz*8192*M_1_PI;
			phase = M_PI/4096;

			for (i=1;i<4096;i++)
			{
			   k = (i<<1)-1;
			   Fourier[k]   = mehr*sin(i*phase) / i;
			   Fourier[k+1] = 0;
			   Inputsignal[i] = 0;

			   Proceed();
			}
			Fourier[8191]=0.0;
			Fourier[8192]=0;
			break;

      case HEAVISIED  :                                              // NÑherungsfunktion der Sprungfunktion
			Initproceed(53086l);
			windowanzahl = 2;

			Fourier[0] = 4./5.;
			Inputsignal[0] = 1;

			mehr = 8./5. * M_PI;

			for(i=1;i<4096;i++)
			{
			   k= (i<<1)-1;
			   phase   = mehr  * i;
			   divisor = M_1_PI/ i;

			   Fourier[k]   = divisor*sin(phase);
			   Fourier[k+1] = divisor*(1.0-cos(phase));
			   Inputsignal[i]=1;

			   Proceed();
			}
			Fourier[8192] = 0;
			Fourier[8191] = 0;
			break;

      case SINEBURST  :                                              // NÑherungfunktion des Sinus-Bursts
			Initproceed(85078l);
			windowanzahl = 3;

			tsin    = 1.0 / F.sinusfrequenz;
			tp      = 1.0 / Unteregrenzfrequenz;
			timp    = F.sinusperioden / F.sinusfrequenz;
			tp_tsin = F.sinusfrequenz / Unteregrenzfrequenz;
			tsin   *= M_PI;

			Fourier[0]     = 0;
			Inputsignal[0] = 0;

			mehr    = 2*M_PI*timp/tp;
			unstetig= (int) (floor(1e5*tp_tsin+.5)/1e5);
			tp_tsin*=tp_tsin;

			dauer   =(int) (timp*2*8192*Unteregrenzfrequenz+1)>>1;

			for(i=1;i<4096;i++)
			{
			   k= (i<<1)-1;
			   phase = mehr *i;

			   if (unstetig!=i)
			   {
			      amplitude   = F.sinusamplitude*tp /
					    (tsin*(tp_tsin - (double) i*i));
			      Fourier[k]  =  amplitude*(1.0-cos(phase));
			      Fourier[k+1]= -amplitude*sin(phase);
			   }
			   else
			   {
			      Fourier[k]  = 0;
			      Fourier[k+1]= F.sinusamplitude*timp/tp;
			   }

			   if (i<=dauer)
			   {
			      Inputsignal[i]=F.sinusamplitude*sin(2*M_PI*i*Minimalerzeitschritt*F.sinusfrequenz);
			   }
			   else
			      Inputsignal[i]=0;
			   Proceed();
			}
			Fourier[8192]=0;
			Fourier[8191]= 0;
			break;

      case USERDEFINED:                                              // Selbst definierte Funktion
			Initproceed(81790l);
			windowanzahl = 2;

			Userdefinedfunction();
			break;
   }

								     // Falte Åbergebene öbertragungsfunktion mit der NÑherung
   Falte_im_freqdomain();
								     // transformiere Frequenz- in den Zeitbereich

   RFFT(FREQUENCYTOTIME);

   if (F.funktionsmode==WATERFALL) Wasserfallberechnen();
								     // suche Maxima der Zeitfunktionen
								     // und normiere die Spektralfunktionen ->DB

   maxy = -1e30;
   miny =  1e30;
								     // Alles Null Normierung unterbinden
   if (fabs(Max_input_spek) <1e-30) Max_input_spek = 1;

   for (i=0;i<4096;i++)
   {
      if (Fourier[i]     > maxy) maxy = Fourier[i];
      if (Inputsignal[i] > maxy) maxy = Inputsignal[i];
      if (Fourier[i]     < miny) miny = Fourier[i];
      if (Inputsignal[i] < miny) miny = Inputsignal[i];

      Spektruminput[i]    = DB(Spektruminput[i]    / Max_input_spek);
      Spektrumoutput[i]   = DB(Spektrumoutput[i]   / Max_input_spek);

      Proceed();
   }

   maxy = floor(10000*maxy*((maxy>0) ? 1.1:0.9)+.5)/10000;
								     // Maxy ungefÑhr Null dann 10% von Miny mit anderem Vorzeichen
   if (fabs(maxy)<1e-5) maxy = - (miny*.1);
   miny = floor(10000*miny*((miny>0) ? 0.9:1.1)+.5)/10000;
								     // Miny ungefÑhr Null dann 10% von Maxy mit anderem Vorzeichen
   if (fabs(miny)<1e-5) miny = - (maxy*.1);
								     // beide Null dann Standard
   if(fabs(maxy)<1e-5)
   {  maxy =  .01;
      miny = -.01;
   }


   Initcalgraph(&fp,windowanzahl,0);		                     // öbergabedatei fÅr Calgraph erstellen

   calewin.rasterfarbe   = LIGHTGRAY;
   calewin.achsenfarbe   = GREEN;
   calewin.kurvenart     = 0; 					     // Kurven
   calewin.kurventeilung = 0; 					     // lineare Teilung
   calewin.anzahl = 2;

   strcpy(calewin.xtext,"t/ms");
   strcpy(calewin.ytext," ");

   calewin.default_maxx=calewin.maxx = 900 * (.5 / Unteregrenzfrequenz);
   calewin.default_minx=calewin.minx = -100* (.5 / Unteregrenzfrequenz);
   calewin.default_maxy=calewin.maxy =  maxy;
   calewin.default_miny=calewin.miny =  miny;

								     // Setze eventuell gespeicherte Zoomwerte

   if ((Cale.zoom_xmax[0]!=0)||(Cale.zoom_xmin[0]!=0))
   {
      calewin.maxx=Cale.zoom_xmax[0];
      calewin.minx=Cale.zoom_xmin[0];
   }

   if ((Cale.zoom_ymax[0]!=0)||(Cale.zoom_ymin[0]!=0))
   {
      calewin.maxy=Cale.zoom_ymax[0];
      calewin.miny=Cale.zoom_ymin[0];
   }


   fwrite(&calewin,sizeof(calewin),1,fp);

   for (i=0;i<4096;i++)						     // berechne die Zeitwerte der StÅtzpunkte
   {
      Fourier[4096+i] = (float) i*1000*Minimalerzeitschritt;
      if (i&31) Proceed();
   }

   calekurve.kurvenfarbe = LIGHTRED;
   strcpy(calekurve.kurventext,"Input Signal");
   calekurve.anzahlwerte = 4096;
   fwrite(&calekurve,sizeof(calekurve),1,fp);

   fwrite(&Fourier[4096],4,4096,fp);
   fwrite(&Inputsignal[0],4,4096,fp);

   calekurve.kurvenfarbe = WHITE;
   strcpy(calekurve.kurventext,"Output Signal");
   fwrite(&calekurve,sizeof(calekurve),1,fp);

   fwrite(&Fourier[4096],4,4096,fp);
   fwrite(&Fourier[0]   ,4,4096,fp);

								     // wenn Sinusburst auch die RMS-Werte berechnen

   if (F.funktionsmode==SINEBURST)
   {

      calewin.default_maxy=calewin.maxy =  10;
      calewin.default_miny=calewin.miny =  -70;

      if ((Cale.zoom_xmax[1]!=0)||(Cale.zoom_xmin[1]!=0))
      {
	 calewin.maxx=Cale.zoom_xmax[1];
	 calewin.minx=Cale.zoom_xmin[1];
      }
      if ((Cale.zoom_ymax[1]!=0)||(Cale.zoom_ymin[1]!=0))
      {
	 calewin.maxy=Cale.zoom_ymax[1];
	 calewin.miny=Cale.zoom_ymin[1];
      }

      calewin.anzahl = 2;
      strcpy(calewin.ytext,"dB");

      fwrite(&calewin,sizeof(calewin),1,fp);

      max_rms=Berechne_RMS(Inputsignal);

								     // normiere RMS-Werte der Eingangsfunktion (Maximum = 0dB)

      for (i=0;i<4096;i++)
      {
	 RMS[i] = DB(RMS[i]/max_rms);
	 if(i&31) Proceed();
      }

								     // Funktionswerte glÑtten

      for (k=0;k<2;k++)
	 for(i=0;i<4094;i++)
	 {
	    RMS[i+1] = (RMS[i]+RMS[i+1]+RMS[i+2])/3;
	    if(i&31) Proceed();
	 }

      calekurve.kurvenfarbe = LIGHTRED;
      strcpy(calekurve.kurventext,"RMS from Input");
      fwrite(&calekurve,sizeof(calekurve),1,fp);

      fwrite(&Fourier[4096],4,4096,fp);
      fwrite(&RMS[0],4,4096,fp);

      Berechne_RMS(Fourier);

								     // normiere RMS-Werte der Antwortfunktion

      for (i=0;i<4096;i++)
      {
	 RMS[i] = DB(RMS[i]/max_rms);
	 if(i&31) Proceed();
      }

								     // Funktionswerte glÑtten

      for (k=0;k<2;k++)
	 for(i=0;i<4094;i++)
	 {
	    RMS[i+1] = (RMS[i]+RMS[i+1]+RMS[i+2])/3;
	    if(i&31) Proceed();
	 }

      calekurve.kurvenfarbe = WHITE;
      strcpy(calekurve.kurventext,"RMS from Output");
      fwrite(&calekurve,sizeof(calekurve),1,fp);

      fwrite(&Fourier[4096],4,4096,fp);
      fwrite(&RMS[0],4,4096,fp);
   }								     // ENDE der Ausgabe der RMS-Werte

								     // öbergabe der Daten fÅr das Spektralfenster

   calewin.kurventeilung = 1; 					     // lineare Teilung
   calewin.anzahl = 3;
   strcpy(calewin.xtext,"Hz");
   strcpy(calewin.ytext,"dB");

   calewin.default_maxx=calewin.maxx = Unteregrenzfrequenz;
   calewin.default_minx=calewin.minx = 4096*Unteregrenzfrequenz;
   calewin.default_maxy=calewin.maxy =  10;
   calewin.default_miny=calewin.miny =  -70;

   if ((Cale.zoom_xmax[2]!=0)||(Cale.zoom_xmin[2]!=0))
   {
      calewin.maxx=Cale.zoom_xmax[2];
      calewin.minx=Cale.zoom_xmin[2];
   }
   if ((Cale.zoom_ymax[2]!=0)||(Cale.zoom_ymin[2]!=0))
   {
      calewin.maxy=Cale.zoom_ymax[2];
      calewin.miny=Cale.zoom_ymin[2];
   }


   fwrite(&calewin,sizeof(calewin),1,fp);

   for (i=0;i<4096;i++)						     // Berechne FrequenzstÅtzpunkte
   {
      Fourier[4096+i] = (float) (i+1)*Unteregrenzfrequenz;
      if (i&63) Proceed();
   }

   calekurve.kurvenfarbe = LIGHTRED;
   strcpy(calekurve.kurventext,"Fre.Response  Input");

   fwrite(&calekurve,sizeof(calekurve),1,fp);
   fwrite(&Fourier[4096],4,4096,fp);
   fwrite(&Spektruminput[0],4,4096,fp);

   for (i=0;i<4096;i++)
   {
								     // berechne die Frequenzamplituden der öbertragungsfunktion

      Inputfunktion_re[i]=DB(hypot(Inputfunktion_re[i],Inputfunktion_im[i]));
      Proceed();
   }

   calekurve.kurvenfarbe = LIGHTBLUE;
   strcpy(calekurve.kurventext,"Transfer Function");

   fwrite(&calekurve,sizeof(calekurve),1,fp);
   fwrite(&Fourier[4096],4,4096,fp);
   fwrite(&Inputfunktion_re[0],4,4096,fp);

   calekurve.kurvenfarbe = WHITE;
   strcpy(calekurve.kurventext,"Fre.Response Output");
   calekurve.anzahlwerte = 4096;

   fwrite(&calekurve,sizeof(calekurve),1,fp);
   fwrite(&Fourier[4096],4,4096,fp);
   fwrite(&Spektrumoutput[0],4,4096,fp);

   fclose(fp);
								     // Speicher aufrÑumen
   Speicherfreigeben(VOIDZ &Spektrumoutput);
   Speicherfreigeben(VOIDZ &Spektruminput);
   Speicherfreigeben(VOIDZ &RMS);
   Speicherfreigeben(VOIDZ &Inputsignal);

   UHR_stop();

   if (spawnl(P_OVERLAY,"CALGRAPH.EXE",NULL)==-1);                   // Aufruf des Graphikprogrammes (Sprung ohne Wiederkehr!)

   remove("UCALGRAP.UEB");
   progrende(OUTOFMEMORYERROR);
}





/***************************************************************************/
/*                                               			   */
/*                            Lese_config                                  */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/*Aufgabe:                                       			   */
/*                                               			   */
/*Einlesen der Konfigurationsdaten aus CALE.CFG und RCALE.UEB              */
/*                                               			   */
/*-------------------------------------------------------------------------*/
/*Parameter:                                     			   */
/*                                               			   */
/*keine                                               			   */
/*                                               			   */
/***************************************************************************/


void near Lese_config()
{
   FILE    *fp;
   char    prognr[12];

   if ((fp=fopen("cale.cfg","rb"))!=NULL)
   {
      fread(&Cale,sizeof(cfgdata),1,fp);

      Unteregrenzfrequenz = Cale.unterefft;
      Timestep            = Cale.timestep;

      fclose(fp);
   }
   else
      progrende(FILEOPENERROR);

   if ((fp=fopen("rcale.ueb","rt"))!=NULL)
   {
      fscanf(fp,"%s",prognr);
      fscanf(fp,"%d",&gdriver);
      if((gdriver != 9) && (gdriver != 3)) progrende(2);
      fscanf(fp,"%d",&gmode);
      if((gmode != 1) && (gmode != 2)) progrende(2);
      fclose(fp);
   }
   else
      progrende(FILEOPENERROR);
}



/***************************************************************************/
/*                                               			   */
/*                            Lese_ubergabedatei                           */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/*Aufgabe:                                       			   */
/*                                               			   */
/*Einlesen der öbergabedatei an FFTRANS                                    */
/*                                               			   */
/*-------------------------------------------------------------------------*/
/*Parameter:                                     			   */
/*                                               			   */
/*keine                                               			   */
/*                                               			   */
/***************************************************************************/


void near Lese_ubergabedatei()
{
   FILE    *fp;
   int     i,zahler=0;

   if ((fp=fopen("ufftrans.ueb","rb"))!=NULL)
   {
      fread(&F,sizeof(inputdata),1,fp);                              // lese Datenstruktur
      fread(&Inputfunktion_gleich,4,1,fp);   			     // lese die GleichwertÅbertragung
      fread(&Inputfunktion_re[0],4,4096,fp);  			     // lese Funktionswerte der öbertragungsfunktion
      fread(&Inputfunktion_im[0],4,4096,fp);
      fclose(fp);

      Proceed_xstart = F.xstart;
      Proceed_ystart = F.ystart;
      Proceed_hoehe  = F.hoehe;

   }
   else
      progrende(FILEOPENERROR);

   for (i=0;i<4096;i++)
   {
      if (fabs(Inputfunktion_re[i])<1e-5) zahler++;
      if (fabs(Inputfunktion_im[i])<1e-5) zahler++;
   }


   remove("UFFTRANS.UEB");
}


/***************************************************************************/
/*                                               			   */
/*                             Userdefinedfunction                         */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/*Aufgabe:                                       			   */
/*                                               			   */
/*Berechne das Zeitsignal der benutzerdefinierten Eingangsfunktion.        */
/*Es werden nur 256 StÅtzpunkte eingegeben. D.h. 15 Punkte interpolieren.  */
/*Berechne die Fourierkoeffizienten dieses Zeitsignals                     */
/*                                               			   */
/*-------------------------------------------------------------------------*/
/*Parameter:                                     			   */
/*                                               			   */
/*keine                                               			   */
/*                                               			   */
/***************************************************************************/



void near Userdefinedfunction()
{
   FILE      *fp;
   punkt     kurve[300];   					     // gleiche Definition wie in CALGEDIT
   int       i,aw;
   punkt     p0,p1;
   double    m0,m1,coe[4];
   double    signalsumme=0;					     // Absolutsumme des Eingangssignals, zum Test ob Signalvorhanden
								     // Die Userdefinedfunction ist in der Datei udfkt.cge von CALGEDIT gespeichert

   if((fp=fopen("udfkt.cge","rb"))==NULL) progrende(FILEOPENERROR);
   fread(&kurve[0],sizeof(kurve),1,fp);
   fclose(fp);

								     // Es werden nur die 1. 256 Werte benîtigt gÅltig sind nur die Y-Werte

   aw=1;
   m1 = 0;
   p1.x=0;
   p1.y=0;

   for(i=0;i<4096;i++)                 				     // bereche Zeitwerte fÅr den halben Zeitbereich
   {
      if((i&15)==0)                    				     // neues Interpolationspolynomen ?
      {
	 p0=p1;
	 m0=m1;

	 p1.x =i+16;

	 if(aw<255)
	 {
	    p1.y =kurve[aw].y;
	    m1 = (kurve[aw].y-kurve[aw-1].y)/16;
	    if (p1.y==kurve[aw+1].y) m1=0;
	 }
	 else
	 {
	    p1.y =0;
	    m1   =0;
	 }

	 aw++;

	 Bestimmepoly(&p0,&p1,m0,m1,coe);			     // bestimme Koeffizienten des Interpolationspolynomens
      }

      signalsumme += fabs(Inputsignal[i] = Fourier[i] = poly(i,3,coe)); // berechne Zeitwert

      Proceed();
   }

   for(;i<8192;i++)						     // Die hÑlfte des Zeitbereiches enthÑlt kein Signal
      Fourier[i] = 0;

   RFFT(TIMETOFREQUENCY);					     // Zeitsignal in den Frequenzbereich transformieren
}





/***************************************************************************/
/*                                               			   */
/*                            Falte_im_freqdomain                          */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/*Aufgabe:                                       			   */
/*                                               			   */
/*Faltung der Eingangsfunktion mit der öbertragungsfunktion                */
/*                                               			   */
/*-------------------------------------------------------------------------*/
/*Parameter:                                     			   */
/*                                               			   */
/*keine                                               			   */
/*                                               			   */
/***************************************************************************/


void near Falte_im_freqdomain()
{
   int       k,i;
   float     wert_re,wert_im,rette_re,rette_im;
   float     spek,sp_re,sp_im;


   Spektruminput[0]   = Fourier[0];
   Spektrumoutput[0]  = Fourier[0] *= Inputfunktion_gleich;


   for(i=1;i<=4096;i++)   					     // fÅr alle benîtigten FrequenzstÅtzpunkte
   {

      k = (i<<1)-1;

      wert_re = Inputfunktion_re[i-1];
      wert_im =-Inputfunktion_im[i-1];

								     // falte Funktion und berechne Spektrum vom Input

      rette_re = Fourier[k];
      rette_im = Fourier[k+1];

      if (Keinefaltung)
      {
	 Fourier[k]  =wert_re;
	 Fourier[k+1]=wert_im;
      }
      else
      {
	 spek = Spektruminput[i-1] = hypot(rette_re,rette_im);
	 if(spek > Max_input_spek) Max_input_spek = spek;

	 sp_re = Fourier[k]  = wert_re*rette_re - wert_im*rette_im;
	 sp_im = Fourier[k+1]= wert_re*rette_im + wert_im*rette_re;

								     // berechne Spektrum des Ergebnisses

	 Spektrumoutput[i-1] = hypot(sp_re,sp_im);
	 Proceed();
      }
   }
}





/***************************************************************************/
/*                                               			   */
/*                            Berechne_RMS                                 */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/*Aufgabe:                                       			   */
/*                                               			   */
/*Berechne den RMS-Wertverlauf einer öbergebenen Zeitfunktion              */
/*                                               			   */
/*-------------------------------------------------------------------------*/
/*Parameter:                                     			   */
/*                                               			   */
/*Zeiger auf die Zeitfunktionswerte                   			   */
/*                                               			   */
/***************************************************************************/


double near Berechne_RMS(float w[])
{
   int laenge;							     // EnthÑlt die Anzahl der StÅtzpunkte Åber die Integriert wird
   int i,j;

   double integral;						     // aktueller Wert des Integrals
   double max;							     // Maximum des RMS-Werteverlaufes

   laenge = (int)(2*8192*Unteregrenzfrequenz/F.sinusfrequenz)>>1;
   laenge ++;

								     // erstes Integral bestimmen

   max = integral=0.0;

   for (j=0;j<laenge;j++)					     // einfaches Aufsummieren der StÅtzpunkte
   {                                                                 // (Verwandschaft zur Z-Transformation)
      integral += w[j]*w[j];
   }

   for (i=0;i<4096;i++)
   {
      j = i + laenge;						     // die neue Summe wird bestimmt, indem ein weiterer StÅtzpunkt
								     // hinzugenommen wird und der zuvor Erste entfernt wird
      if ((RMS[i] = sqrt(integral/laenge))>max) max=RMS[i];
      integral -= w[i]*w[i];
      if (integral<0) integral = 0;
      if (j<4096) integral += w[j]*w[j];

      Proceed();
   }

   return(max);
}





/***************************************************************************/
/*                                               			   */
/*                            Wasserfallberechnen                          */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/*Aufgabe:                                       			   */
/*                                               			   */
/*Berechne das Wasserfalldiagramm                                          */
/*                                               			   */
/*-------------------------------------------------------------------------*/
/*Parameter:                                     			   */
/*                                               			   */
/*keine                                               			   */
/*                                               			   */
/***************************************************************************/


void near Wasserfallberechnen()
{
   FILE      *fp;
   float     endtime,df;
   char      dummy[20],*ptr;
   struct    date d;
   struct    time tv;
   char      month[13][4] = {"","JAN","FEB","MAR","APR","MAY",
			     "JUN","JUL","AUG","SEP","OCT","NOV","DEC"};

   int       i,timedecay,k,t,zeit;
   float     amplitude[120],timevalue[120];
   float     max;

   char      text_graph[8][20];                                      // Neu eingefÅgt 21.5.96

								     // Dieser Speicher wird nicht benîtigt
   Speicherfreigeben(VOIDZ &Spektrumoutput);
   Speicherfreigeben(VOIDZ &Spektruminput);
   Speicherfreigeben(VOIDZ &RMS);
   Speicherfreigeben(VOIDZ &Inputsignal);

								     // bilde StÅtzpunkte fÅr die Ausgabekurven

   zeit = 1;
   Pointsw = 5;

   for (i=0;i<6;i++) Stuetze[i]=i+1;

   for (i=1;i<4096;i++)
   {
      if ((Unteregrenzfrequenz*i>Unteregrenzfrequenz*pow(2,zeit/13.0))&&(zeit<154)&&(i>6))
      {
	 Stuetze[Pointsw++]=i;
	 while(Unteregrenzfrequenz*(i+1)>Unteregrenzfrequenz*pow(2,zeit/13.0)) zeit++;
      }
   }

   endtime = ceil((((float) Timestep*100)*Minimalerzeitschritt*10000+.5))/10;

								     // öbergabedatei fÅr Wasserfalldiagram

   if((fp=fopen("UCALWAFA.UEB","wb"))==NULL) progrende(FILEOPENERROR);

   setvbuf(fp,NULL,_IOFBF,32766);			             // 32766 ist der grî·t mîgliche Dateipuffer


   fwrite("Waterfall Diagram",1,20,fp);
   fwrite(F.artkommentar,1,20,fp);
   fwrite("of",1,20,fp);
   strcpy(dummy,F.name);
   if((ptr=strchr(dummy,'.'))!=NULL) *(ptr) = '\0';
   fwrite(dummy,1,20,fp);

   strcpy(text_graph[0],F.bezeich);
   strcpy(text_graph[1],dummy);
   strcpy(text_graph[2],"comment:");
   strcpy(text_graph[3],F.kommentar);
   strcpy(text_graph[4],"");
   strcpy(text_graph[5],F.artkommentar);
   strcpy(text_graph[6],"calculated at:");

   getdate(&d);
   gettime(&tv);
   sprintf(dummy,"%02.2d:%02.2d  %s.%02.2d.%d",tv.ti_hour,tv.ti_min,month[d.da_mon],d.da_day,d.da_year);
   strcpy(text_graph[7],dummy);

   fwrite(text_graph,sizeof(text_graph),1,fp);

								     // Einheiten Åbergeben
   fwrite("msec",sizeof(char),6,fp);
   fwrite("Hz",sizeof(char),6,fp);
   fwrite("dB",sizeof(char),6,fp);

   fwrite(&gdriver,sizeof(int),1,fp);
   fwrite(&gmode,sizeof(int),1,fp);

								     // x-Minimum und Maximum
   df=0.0;
   fwrite(&df,     sizeof(float),1,fp);
   fwrite(&endtime,sizeof(float),1,fp);

								     // y-Minimum und Maximum
   df = Unteregrenzfrequenz*2;
   fwrite(&df,sizeof(float),1,fp);
   df = 2000*Unteregrenzfrequenz;
   fwrite(&df,     sizeof(float),1,fp);

								     // z-Minimum und Maximum
   df = -50.0;
   fwrite(&df,sizeof(float),1,fp);
   df = 0.0;
   fwrite(&df,sizeof(float),1,fp);

								     // Kurvenarten Åbergeben

   i=0;
   fwrite(&i,sizeof(int),1,fp);
   i=1;
   fwrite(&i,sizeof(int),1,fp);
   i=0;
   fwrite(&i,sizeof(int),1,fp);

								     // Anzahl der Kurven Åbergeben
   i=100;
   fwrite(&i,sizeof(int),1,fp);


   for (i=0;i<Pointsw;i++) timevalue[i] = Stuetze[i]*Unteregrenzfrequenz;

   timedecay = Timestep;

   max=1e-30;
								     // Bereinigung von Rundungsfehlern der FFT
   for (i=0;i<8192;i++) if(fabs(Fourier[i])>max) max=fabs(Fourier[i]);

   max /= 1000000.0;

   for (i=0;i<8192;i++) if(fabs(Fourier[i])<max) Fourier[i]=0;


   for (int kurven=0;kurven<100;kurven++)			     // FÅr alle 100 darzustellenden Kurven
   {
      if (kurven!=0)
      {
	 for (i=0;i<timedecay;i++)
	 {
	    Fourier[i]=0;
	    Fourier[8191-i]=0;
	 }

	 Fourier[timedecay+0]*=0.05;				     // Windowing
	 Fourier[timedecay+1]*=0.2;
	 Fourier[timedecay+2]*=0.5;
	 Fourier[timedecay+3]*=0.8;
	 Fourier[timedecay+4]*=0.95;
	 Fourier[8191-timedecay]*=0.05;
	 Fourier[8190-timedecay]*=0.2;
	 Fourier[8189-timedecay]*=0.5;
	 Fourier[8188-timedecay]*=0.8;
	 Fourier[8187-timedecay]*=0.95;

	 timedecay+=Timestep;
      }

      Callproceed=1;						     // Ein Arbeitsbalken Zeichnen
      Proceed();
      Callproceed=0;

      RFFT(TIMETOFREQUENCY);

      for (i=0;i<Pointsw;i++)
      {
	 k  = 2*Stuetze[i]-1;
	 df = hypot(Fourier[k],Fourier[k+1]);
								     // alles kleiner -88 db soll null sein (Auflîsungsvermîgen)
	 if (df<4e-5) amplitude[i] = -100;  			     // so null entsprechen
	 else         amplitude[i] = DB(df);
      }

      for(t=i=0;i<Pointsw;i++) if(amplitude[i]<-99.99) t++;          // schon alles Funktionswerte kleine -99.99dB ?


      if (t==Pointsw) 					             // keine Zeitfunktionswerte mehr
      {
	 Callproceed=1;
	 for (;kurven<100;kurven++)
	 {
	    df=(kurven*Timestep*Minimalerzeitschritt*1000.0);
	    Proceed();
	    fwrite(&Pointsw,sizeof(int),1,fp);
	    fwrite(&df,sizeof(float),1,fp);
	    Proceed();
	    fwrite(&timevalue[0],sizeof(float),Pointsw,fp);
	    fwrite(&amplitude[0],sizeof(float),Pointsw,fp);
	 }

	 break;
      }

      if (Inputfunktion_gleich!=0.0) Fourier[0]=0;		     // Gleichrichtwertkorrektur fals nîtig

      Callproceed=1;
      Proceed();
      Callproceed=0;
      RFFT(FREQUENCYTOTIME);                                         // Funktionswerte zurÅcktransformieren

      if (kbhit())						     // soll die Berechnung abgebrochen werden ? geÑndert 21.5.96
	 if(check_getch()==ESC) break;


								     // Aktueller Zeitwert der Kurve
      df=(kurven*Timestep*Minimalerzeitschritt*1000.0);

      fwrite(&Pointsw,sizeof(int),1,fp);
      fwrite(&df,sizeof(float),1,fp);

      fwrite(&timevalue[0],sizeof(float),Pointsw,fp);
      fwrite(&amplitude[0],sizeof(float),Pointsw,fp);
   }  								     // nÑchste Kurve berechnen

   fclose(fp);

   UHR_stop();

   if (kurven==100)						     // Berechnung nicht abgebrochen ?
      if (spawnl(P_OVERLAY,"CALWAFAL.EXE",NULL)==-1)                 // Aufruf des Graphikprogrammes fÅr das Wasserfalldiagramm
      {                                                              // geÑndert 10.10.96 von P_WAIT -> spart Speicher und ist gut !
	  remove("UCALWAFA.UEB");				     // öbergabedatei lîschen
	  if (errno==ENOMEM) progrende(OUTOFMEMORYERROR);
	  progrende(FILEOPENERROR);
      }

   progrende(0);
}







/***************************************************************************/
/*                                                                         */
/*                                                                         */
/*                Funktionen um CALGRAPH aufzurufen                        */
/*                                                                         */
/*                                                                         */
/***************************************************************************/




void near Initcalgraph(FILE **fp,int windows,int sort)
{
   calgraph_init caleinit;
   char *ptr;
   char month[13][4] = {"","JAN","FEB","MAR","APR","MAY","JUN",
			   "JUL","AUG","SEP","OCT","NOV","DEC"};
   struct date d;
   struct time t;



   caleinit.gdriver = gdriver;
   caleinit.gmode   = gmode;
   caleinit.windows = windows;
   caleinit.sort    = sort;
								      // Aufbau der Hardcopykommentare
   strcpy(caleinit.hardcopytext[0],F.bezeich);
   strcpy(caleinit.hardcopytext[1],F.name);
   if((ptr=strchr(caleinit.hardcopytext[1],'.'))!=NULL) *(ptr) = '\0';
   strcpy(caleinit.hardcopytext[2],"comment:");

   strcpy(caleinit.hardcopytext[3],F.kommentar);
   strcpy(caleinit.hardcopytext[4],"");

   strcpy(caleinit.hardcopytext[5],F.artkommentar);
   strcpy(caleinit.hardcopytext[6],"calculated at");
   getdate(&d);
   gettime(&t);
   sprintf(caleinit.hardcopytext[7],"%02.2d:%02.2d  %s.%02.2d.%d",t.ti_hour,t.ti_min,month[d.da_mon],d.da_day,d.da_year);

   if((*fp=fopen("UCALGRAP.UEB","wb"))==NULL) progrende(1);
   fwrite(&caleinit,sizeof (caleinit),1,*fp);
}





/***************************************************************************/
/*                                               			   */
/*                               progrende                                 */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/*Aufgabe:                                       			   */
/*                                               			   */
/*Einsprung bei Fehler oder verlassen des Programmes                       */
/*                                               			   */
/*-------------------------------------------------------------------------*/
/*Parameter:                                     			   */
/*                                               			   */
/*int fehlercode                                     			   */
/*                                               			   */
/***************************************************************************/

void near progrende(int error)
{

   Speicherfreigeben(VOIDZ &Spektrumoutput);
   Speicherfreigeben(VOIDZ &Spektruminput);
   Speicherfreigeben(VOIDZ &RMS);
   Speicherfreigeben(VOIDZ &Inputsignal);

   UHR_stop();

   exit(error);
}




/***************************************************************************/
/*                                               			   */
/*                               Bestimmepoly                              */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/*Aufgabe:                                       			   */
/*                                               			   */
/*Besimme die Polynomenkoeffizienten fÅr Interpolation als BeziÇr-Kurve    */
/*                                               			   */
/*-------------------------------------------------------------------------*/
/*Parameter:                                     			   */
/*                                               			   */
/*Zeiger auf erster StÅtzpunkt und zweiter StÅtzpunkt                      */
/*Steigungen der StÅtzpunkte                     			   */
/*Zeiger auf Koeffizientenarry                   			   */
/*                                               			   */
/***************************************************************************/


void near Bestimmepoly(punkt *p0,punkt *p1,double m0,double m1,double coe[])
{
    double matrix[4][4];
    double loesung[4];
    double faktor;

    int i,j,k;

								     // Aufbauen der Gleichungssystemmatrix

    matrix[0][0] = matrix[1][0] = matrix[2][1] = matrix[3][1] = 1.0;
    matrix[2][0] = matrix[3][0] = 0.0;

    matrix[2][2] = 2*(matrix[0][1] = p0->x);
    matrix[2][3] = 3*(matrix[0][2] = p0->x*p0->x);
    matrix[0][3] = p0->x*matrix[0][2];

    matrix[3][2] = 2*(matrix[1][1] = p1->x);
    matrix[3][3] = 3*(matrix[1][2] = p1->x*p1->x);
    matrix[1][3] = p1->x*matrix[1][2];

								     // Aufbauen des Loesungsvektors

    loesung[0] = p0->y;
    loesung[1] = p1->y;
    loesung[2] = m0;
    loesung[3] = m1;

								     // lîse Gleichungssystem mit Gau·

    for (i=0;i<4;i++)
       for(k=i+1;k<4;k++)
	  if(matrix[k][i]!=0)
	  {
	     faktor = matrix[k][i]/matrix[i][i];
	     for (j=i+1;j<4;j++) matrix[k][j]-=faktor*matrix[i][j];
	     loesung[k]-=faktor*loesung[i];
	  }

								     // RÅcksubstitution und Ergebnis öbertragen

    for (i=3;i>=0;i--)
    {
       coe[i] = faktor = loesung[i]/matrix[i][i];
       for (j=i-1;j>=0;j--) loesung[j]-=faktor*matrix[j][i];
    }
}

/***************************************************************************/
/*                                               			   */
/*                       Funktionen fÅr den Rechenzeitbalken               */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/*Aufgabe:                                       			   */
/*                                               			   */
/*Zeichen des Rechenzeitbalkens                                            */
/*                                               			   */
/*-------------------------------------------------------------------------*/
/*Parameter:                                     			   */
/*                                               			   */
/*verschiedene                                        			   */
/*                                               			   */
/***************************************************************************/


void near pascal Initproceed(long max)
{
   Proceed_max     = max;
   Proceed_aktuell = -1;
}


void far pascal Proceed()
{
   int jetzt,x,y,h;

   if (Callproceed)
   {
      Proceed_done++;

      jetzt = (((int)(F.laenge * ((double) Proceed_done / Proceed_max))<<1)+1)>>1;

      if (jetzt>Proceed_aktuell)
      {
	 Proceed_aktuell = jetzt;

	 x = Proceed_xstart+jetzt;
	 y = Proceed_ystart;

	 UHR_draw = NO;

	 for (h=0;h<Proceed_hoehe;h++)
	 {
	    asm mov al,byte ptr [Proceed_farbe];
	    asm mov ah,0x0c;
	    asm mov bh,0;
	    asm mov dx,[y];
	    asm mov cx,[x];
	    asm int 0x10;
	    y++;
	 }

	 UHR_draw = YES;
      }
   }
}





/***************************************************************************/
/*                                               			   */
/*                                    DB                                   */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/*Aufgabe:                                       			   */
/*                                               			   */
/*Umrechnung Wert nach dB                                                  */
/*                                               			   */
/*-------------------------------------------------------------------------*/
/*Parameter:                                     			   */
/*                                               			   */
/*double wert                                         			   */
/*                                               			   */
/***************************************************************************/


float near pascal DB(double wert)
{
   if(wert<1e-50) return(-1000); 				     // festgelegte Auflîsungsgrenze
   return (20*log10(wert));
}





/***************************************************************************/
/*                                               			   */
/*                             check_getch                                 */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/*Aufgabe:                                       			   */
/*                                               			   */
/*Tastatur lesen                                                           */
/*                                               			   */
/*-------------------------------------------------------------------------*/
/*Parameter:                                     			   */
/*                                               			   */
/*keine                                               			   */
/*                                               			   */
/***************************************************************************/


int  near pascal check_getch()
{
   int input;

   input = getch();
   if (input == 0)   input = getch() + 500;
   if (input == 27)
   {
      while(inportb(0x0060) < 0x80);		     	             /* Warten auf Release-Code (Taste losgelassen)   */

								     /* Pointer gleichsetzen -> Tastaturpuffer leeren */
      *((unsigned int far*) MK_FP(0x0040, 0x001A)) = *((unsigned int far*) MK_FP(0x0040, 0x001C));
   }

   return(input);
}





/***************************************************************************/
/*                                               			   */
/*                         Speicherfreigeben                               */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/*Aufgabe:                                       			   */
/*                                               			   */
/*Speicher auf dem farheap, der belegt ist, freigeben                      */
/*                                               			   */
/*-------------------------------------------------------------------------*/
/*Parameter:                                     			   */
/*                                               			   */
/*void far far *block : Zeiger auf den Zeiger des Speichers                */
/*                                               			   */
/***************************************************************************/


void pascal near Speicherfreigeben(void far * far *block)
{

   if (*block!=NULL)						     // Speicher nur freigeben wenn noch belegt
   {
      farfree(*block);
      *block=NULL;
   }
}

#endif