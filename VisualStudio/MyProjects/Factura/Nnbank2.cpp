#include<conio.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<graphics.h>
#include<dos.h>
#include<math.h>


/***************************************************************************/
/*           Struktur Deklaration fÅr die Standard Eingabefunktion         */
/***************************************************************************/

struct eingstruct
{
// Farbdefinition fÅr die Eingabefunktion

int fravofarb;       						     // Textfarbe        fÅr den Fragetext
int frahifarb;       						     // Hintergrundfarbe fÅr das Fragefeld
int antvofarb;       						     // Textfarbe        fÅr den Antworttext
int anthifarb;       						     // Hintergrundfarbe fÅr das Eingabefeld
int cursorvofarb;    						     // Cursorfarbe      fÅr das Eingabefeld

// Steuerparameter fÅr die Eingabefunktion                                

int ab;              						     // Parameter fÅr Art des Eingabeabschlusses
								     // = 0, nur mit RETURN
								     // = 1, mit RETURN und  CURSOR DOWN
								     // = 2, mit RETURN und  CURSOR UP/DOWN
								     // = 3, mit RETURN und  CURSOR UP/DOWN    und PAGE UP/DOWN
								     // = 4, mit RETURN alle CURSOR-Bewegungen und PAGE UP/DOWN 

int stop;            						     // = 0, kein Abbruch mit ESC zugelassen
								     // = 1,      Abbruch mit ESC zugelassen

int mode;            						     // = 0, mit  Eingabe
								     // = 1, ohne Eingabe (nur Antwort vorlegen )

int blocklen;        						     // Laenge des farbigen Fragetexthintergrundes
								     // = 0 --> Autojustierung entsprechend der FragetextlÑnge
								     // = 0 und Fragetext=Leerstring --> keine Fragetextausgabe
int font;            						     // = 5 ohne Schatten benutzt SMALL_FONT und positioniert
								     // auf Pixelbasis, alle anderen Werte CALE font     
};



/***************************************************************************/
/*           Functionsprototypen  fÅr die Standard Eingabefunktion         */
/***************************************************************************/

int Eingabe(int spalte, int zeile, char *eingtext, int eingparam,
	    int einglen, void *eingwert, eingstruct *param);

int pascal check_getch();
void Calerahm(char *s);
void Button(int x1, int y1, int x2, int y2, int colorfill, int colorleft);


int gmode=1,gdriver=9;
struct adressen
{
   char vorname[16],
	nachname[16],
	strasse[30],
	plz[12],
	ort[20],
	land[30],
	kennung[12],
	lft[2];
};

adressen Adresse;
eingstruct eing  = {   WHITE,CYAN,
		       WHITE,BROWN,
		       LIGHTGREEN,     // Vorbelegung der Eingabestructur als Standart
		       2,1,1,0,0};     // Eingabe abschlu· mit RETURN und CUSOR UP/DOWN ESC=YES 
				       // MODE=KEINE EINGABE ,blocklen und font = 0;

char print[40];
double rnr=0;
double eingdat;
int DM_betrag, Pfennige;
char null[2]="",*ptr;
int zeile,e;



void out(char *s)
{  int i=0;
   char k[2]="p";

   while(s[i]) 
   { 
      k[0]=s[i++];
      fprintf(stdprn,"%s ",k);
   }
   fprintf(stdprn,"\n\n"); 
}


void main()
{  

   long i;

   registerbgidriver(EGAVGA_driver);
   setgraphbufsize(100);
   initgraph(&gdriver,&gmode,"\0");

      setbkcolor(BLUE);
      cleardevice();
      eing.frahifarb = CYAN;

      Calerahm("Ausdruck Nachnahmeetiketten");

      eing.mode=0;
      eing.blocklen=40;


      e = Eingabe(5,10,"Anzahl Etiketten eingeben :",2,4,&rnr,&eing);
      if (e==8) 
      {
         closegraph();
         exit(1);
      }
      else
      {
         e = Eingabe(5,12,"DM-Betrag eingeben :",2,4,&eingdat,&eing);
         DM_betrag = (int) floor(eingdat+.1);
         if (e==8) 
         {
            closegraph();
            exit(1);
         }

         eingdat = 0.0;
         e = Eingabe(5,14,"Pfennige eingeben :",2,2,&eingdat,&eing);
         Pfennige = (int) floor(eingdat+.1);
         if (e==8) 
         {
            closegraph();
            exit(1);
         }
      }

      for(i=0;i<rnr;i++)
      	 {
	    fprintf(stdprn,"    Commerzbank Kiel\n\n");
	    fprintf(stdprn,"  Bankleitzahl 21040010\n");
	    fprintf(stdprn,"    Konto-Nr. 7480734\n\n");
           if(Pfennige == 0)
              fprintf(stdprn,"         DM %d,00", DM_betrag);
            else if(Pfennige < 10)
               fprintf(stdprn,"         DM %d,0%d", DM_betrag, Pfennige);
            else
               fprintf(stdprn,"         DM %d,%d", DM_betrag, Pfennige);
            fprintf(stdprn,"\n\n\n\n\n\n\n");
	 }
  closegraph();
}


/***************************************************************************/
/*                                                                         */
/*                          Standard Eingabefunktion                       */
/*                                                                         */
/*                (C)  ELAC Technische Software, Kiel  1994		   */
/*                          Ver. 2.0        02.08.94			   */
/*                                                                         */
/* Graphikmodi:      gdriver = 3 (9), gmode = 1 (1 oder 2)                 */
/*                   Achtung gmode wird als globale Variable vorausgesetzt,*/
/*                   Funktion benîtigt die Font-Datei CALE.CHR             */
/*            								   */
/* Aufruf:  rueck=Eingabe(eingspal,eingzeile,"Frage",eingparam,            */
/*                        einglen,&eingwert,&eing);	                   */
/*                                                                         */
/* öbergabeparameter:                                                      */
/* eingspal : Eingabespalte (1...80), Spaltenbreite = 8 Pixels             */
/* eingzeil : Eingabezeile (1...23 (24)), Zeilenhîhe = 15 (20) Pixels      */
/*            mit font=5 sind es Pixelpositionen !                         */
/* Fragetext: <= 75 Zeichen, nicht laenger als blocklen in eingstruct      */
/* eingparam: = 0 (Texteingabe)                  (char)                    */
/*            = 1 (Dateneingabe mit +/- Vorz.)   (double)                  */
/*            = 2 (Dateneingabe ohne Vorzeichen) (double)                  */
/*            = 3 (Dateinameneingabe)            (char)                    */
/*            = 4 (Nur Cursorbewegung entsprechend eingstruct auswerten)   */
/* einglen  : max. Zeichenanzahl der Eingabevariable (<= 75)               */
/*            einglen = 0 --> keine text- oder Zahleneingabe               */
/*                                                                         */
/* Zeiger auf Eingabevariable : z.B.  Name  oder &Zahlenwert               */
/* Zeiger auf eingstruct      : z.B.  &eing : eingstruct wird bei der      */
/*                                            Deklaration beschrieben.     */
/*                                                                         */
/* Die Art des Eingabeabschlusses oder des Abbruchs wird als               */
/* Funktionswert zurÅckgegeben (bei eing.mode = 1 --> return(NULL)).       */
/* RÅckgabewerte (Return(x)):                                              */
/*    0,  alles ok, Abschluss mit Return bzw. eing.mode = 1                */
/*    1,  Abschluss mit Cursor down                                        */
/*    2,  Abschluss mit Cursor up                                          */
/*    3,  Abschluss mit Page down                                          */
/*    4,  Abschluss mit Page up                                            */
/*    5,  Abschluss mit Cursor rechts                                      */
/*    6,  Abschluss mit Cursor links                                       */
/*    8,  Abbruch mit ESC                                                  */
/***************************************************************************/



/* lokale Funktionen, werden nicht vorweg deklariert */

void ausgabe_eingstr(int x_pos, int y_pos, char *s, int backgrlaenge, int backgrhoehe, int einglen, eingstruct *param)
{
   int  i=0, x;
   char k[2];

   if(einglen > 0)						     /* Ausgabe erfolgt nur, wenn der Parameter Einglen > 0 ist. */
   {
      setfillstyle(1, param->anthifarb);
      bar(x_pos,y_pos,x_pos+backgrlaenge-1,y_pos+backgrhoehe-1);     /* Eingabestring lîschen */

      if (param->font == 5)   y_pos++;
      else y_pos-=2;
      k[1] = '\x0';
      while(*s)
      {
	 k[0] = s[0];						     /* damit die Cursorposition korrekt mîglich ist */
	 setcolor(DARKGRAY);
	 x = x_pos + i*8 + 4;
	 if (param->font!=5)
	    outtextxy(x+1,y_pos+1,k);
	 setcolor(param->antvofarb);
	 outtextxy(x,y_pos,k);
	 i++;
	 s++;
      }
   }
}


void pascal eingbeep(){sound(1000);delay(100);nosound();}





/* globale Funktion 'check_getch()' */

int pascal check_getch()
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





/* globale Funktion 'Eingabe' */

int Eingabe(int eingspal,int eingzeil,char *eingtext,int eingparam,
	    int einglen,void *eingwert,eingstruct *param)
{
   int    backgrhoehe, backgrlaenge, fonthoehe;
   int    i, j, k, blocklen, user;
   int    x_lipos_backgr, y_obpos_backgr;
   int    eingstrlaenge, input, abschluss = 10;
   int    x_cursor, y_cursor;
   int    screenbuffer[10];
   char   eingstr[80] = " ", *endptr, dummych;
   unsigned char   leerzeichen;
   double *einptr, eingdat;


   if(einglen > 75)   einglen = 75;
   if(strlen(eingtext) > 75)   eingtext[75] = '\x0';

   if(gmode == 2)
   {
      fonthoehe = 16;
      backgrhoehe = 20;
   }
   else
   {
      fonthoehe = 12;
      backgrhoehe = 15;
   }

   if (param->font != 5)
   {
      user = installuserfont("CALE");
      setusercharsize(8,8,fonthoehe,9);
      x_lipos_backgr = eingspal*8 - 4;
      y_obpos_backgr = (eingzeil - 1)*backgrhoehe;
   }
   else 
   {
      user = SMALL_FONT;
      backgrhoehe = 2 + fonthoehe;
      setusercharsize(8,6,fonthoehe,12);
      x_lipos_backgr = eingspal;				     /* eingspal hier Pixel-# in x-Richtung */
      y_obpos_backgr = eingzeil;				     /* eingzeil hier Pixel-# in y-Richtung */
   }

   settextstyle(user,HORIZ_DIR,0);
   settextjustify(LEFT_TEXT,TOP_TEXT);

   blocklen = param->blocklen;
   if(blocklen > 75)   blocklen = 75;
   if((blocklen == 0) && (strlen(eingtext) > 0))
   {
      strcpy(eingstr,eingtext);
      strcat(eingstr,"m");
      backgrlaenge = textwidth(eingstr);
   }
   else
      backgrlaenge = 8*blocklen;

   if(backgrlaenge > 0)				   		     /* Hintergrund Fragetext ausmalen */
   {
      setfillstyle(1,param->frahifarb);
      bar(x_lipos_backgr,y_obpos_backgr,x_lipos_backgr+backgrlaenge-1,
	  y_obpos_backgr+backgrhoehe-1);			     /* Fragetext lîschen */

      if(param->font != 5)
      {
	 setcolor(DARKGRAY);
	 outtextxy(x_lipos_backgr+5,y_obpos_backgr-1,eingtext);
	 setcolor(param->fravofarb);
	 outtextxy(x_lipos_backgr+4,y_obpos_backgr-2,eingtext);
      }
      else
      {
	 setcolor(param->fravofarb);
	 outtextxy(x_lipos_backgr+4,y_obpos_backgr+1,eingtext);
      }
   }

   if(backgrlaenge > 0)
      x_lipos_backgr = x_lipos_backgr + backgrlaenge + 8;            /* Hintergrund Antwort ausmalen     */
   backgrlaenge = (einglen + 1)*8;                                   /* und vorgeladene Antwort ausgeben */

   if(eingparam == 3)   leerzeichen = '\xff';
   else   leerzeichen = ' ';

   if (einglen > 0)
   {
      if((eingparam == 1) || (eingparam == 2))
      {
	 einptr  = (double *) eingwert;
	 eingdat = *einptr;
	 gcvt(eingdat, einglen, eingstr);
	 eingstr[einglen] = '\x0';
      }
      if((eingparam == 0) || (eingparam > 2))
      {
	 strcpy(eingstr,(char *) eingwert);
	 eingstr[einglen] = '\x0';
	 if(eingparam == 3)					     /* nur MS-DOS gÅltige Zeichen zulassen */
	 {
	    if(strlen(eingstr) == 0)   strcpy(eingstr, "\xff");
	    else
	    {
	       for(k = 0; k < strlen(eingstr); k++)
	       {
		  dummych = eingstr[k];
		  if((dummych!=33)&&(dummych!=45)&&(dummych!=125)&&
		     (dummych!=126)&&(dummych!=255)&&((dummych<35)||(dummych>41))&&
		     ((dummych<48)||(dummych>57))&&((dummych<64)||
		     (dummych>90))&&((dummych<94)||(dummych>123)))
		  {     
		     eingstr[k] = leerzeichen;
		  }
	       }
	    }
         }
      }
      ausgabe_eingstr(x_lipos_backgr, y_obpos_backgr, eingstr,
		      backgrlaenge, backgrhoehe, einglen, param);
   }

   eingstrlaenge = strlen(eingstr);
   if(param->mode == 1)   abschluss = 0;                             /* Es erfolgt keine Eingabe */
   i = 0;
   j = 0;

   x_cursor = x_lipos_backgr + 4;
   y_cursor = y_obpos_backgr + backgrhoehe - 2;
   

   for(k = 0; k < 8; k++)   screenbuffer[k] = getpixel(x_cursor + k, y_cursor);

   while(abschluss >= 10)
   {
      if (einglen > 0)
      {
	 x_cursor = x_lipos_backgr + j*8 +4;			     /* Cursor lîschen */
	 setcolor(param->anthifarb);
	 line(x_cursor,y_cursor+1,x_cursor+7,y_cursor+1);
	 
	 for(k = 0; k < 8; k++)
	    if(getpixel(x_cursor + k, y_cursor) == param->cursorvofarb)
	       putpixel(x_cursor + k, y_cursor, screenbuffer[k]);
	 x_cursor = x_lipos_backgr + i*8 +4;                         /* Cursor ausgeben */
	 for(k = 0; k < 8; k++)   screenbuffer[k] = getpixel(x_cursor + k, y_cursor);
	 setcolor(param->cursorvofarb);
	 line(x_cursor,y_cursor,x_cursor+7,y_cursor);
	 line(x_cursor,y_cursor+1,x_cursor+7,y_cursor+1);
	 for(k = 0; k < 8; k++)					     /* Zeichen hinter dem Cursor wiederherstellen */
	    if(screenbuffer[k] != param->anthifarb)   putpixel(x_cursor + k, y_cursor, screenbuffer[k]);
      }

      j = i;

      input = check_getch();

      switch(input)
      {
	    case 575:                                                /* Cursor left */
	       if(param->ab == 4)   abschluss = 6;
	       else
	       {
		  if(i > 0)    i = i - 1;
		  else    eingbeep();
	       }
	       break;
	    case 577:                                                /* Cursor right */
	       if(param->ab == 4)   abschluss = 5;
	       else
	       {
		  if(i == eingstrlaenge)
		  {
		     eingstr[i] = leerzeichen;
		     eingstrlaenge = eingstrlaenge + 1;
		     eingstr[eingstrlaenge] = '\x0';
		     if(i < einglen - 1)    i = i + 1;
		     else    eingbeep();
		     if(eingparam == 3)
			ausgabe_eingstr(x_lipos_backgr, y_obpos_backgr, eingstr,
					backgrlaenge, backgrhoehe, einglen, param);
		  }
		  else
		  {
		     if(i < einglen - 1)    i = i + 1;
		     else    eingbeep();
		  }
	       }
	       break;
	    case 571:                                                /* Pos 1 */
	       i = 0;
	       break;
	    case 579:                                                /* Stringende */
	       if(eingstrlaenge < einglen)    i = eingstrlaenge;
	       else    i = einglen - 1;
	       break;
	    case 580:                                                /* Cursor down */
	       if(param->ab >= 1)    abschluss = 1;
	       else    eingbeep();
	       break;
	    case 572:                                                /* Cursor up */
	       if(param->ab >= 2)    abschluss = 2;
	       else    eingbeep();
	       break;
	    case 581:                                                /* Page down */
	       if(param->ab >= 3)    abschluss = 3;
	       else    eingbeep();
	       break;
	    case 573:                                                /* Page up */
	       if(param->ab >= 3)    abschluss = 4;
	       else    eingbeep();
	       break;
	    case 582:                                                /* Insert */
	       if(eingparam > 3)   break;
	       if(i < eingstrlaenge)
	       {
		  for(k = eingstrlaenge; k > i; k--)
		     eingstr[k] = eingstr[k - 1];
		  eingstr[i] = leerzeichen;
		  if(eingstrlaenge < einglen)
		     eingstrlaenge = eingstrlaenge + 1;
		  eingstr[eingstrlaenge] = '\x0';
		  ausgabe_eingstr(x_lipos_backgr, y_obpos_backgr, eingstr,
				  backgrlaenge, backgrhoehe, einglen, param);
	       }
	       break;
	    case 583:                                                /* Delete */
	       if(eingparam > 3)   break;
	       if((i == 0) && (eingstrlaenge == 1))
	       {
		  eingstr[0] = leerzeichen;
		  ausgabe_eingstr(x_lipos_backgr, y_obpos_backgr, eingstr,
				  backgrlaenge, backgrhoehe, einglen, param);
	       }
	       if((eingstrlaenge > 1) && (i < eingstrlaenge))
	       {
		  for(k = i; k < eingstrlaenge; k++)
		     eingstr[k] = eingstr[k + 1];  
		  eingstrlaenge = eingstrlaenge - 1;
		  ausgabe_eingstr(x_lipos_backgr, y_obpos_backgr, eingstr,
				  backgrlaenge, backgrhoehe, einglen, param);
	       }
	       break;
	    case 8:                                                  /* Back Space */
	       if(eingparam > 3)   break;
	       if(((i == 1) || (i == 0)) && (eingstrlaenge == 1))
	       {
		  eingstr[0] = leerzeichen;
		  ausgabe_eingstr(x_lipos_backgr, y_obpos_backgr, eingstr,
				  backgrlaenge, backgrhoehe, einglen, param);
	       }
	       if((i > 0) && (eingstrlaenge > 1) && (i <= eingstrlaenge))
	       {
		  for(k = i - 1; k < eingstrlaenge; k++)
		  eingstr[k] = eingstr[k + 1];  
		  eingstrlaenge = eingstrlaenge - 1;
		  ausgabe_eingstr(x_lipos_backgr, y_obpos_backgr, eingstr,
				  backgrlaenge, backgrhoehe, einglen, param);
	       }
	       if(i > 0)    i = i - 1;
	       break;
	    case 13:                                                 /* Return */
	       abschluss = 0;
	       break;
	    case 27:                                                 /* ESC */
	       if(param->stop == 1) return(8);
	    default:                                                 /* Zahl oder Texteingabe */
	       dummych =(char) input;
	       switch (eingparam)
	       {
								     /* Filenamen Test: MS-DOS gÅltig ? */
		  case 3: if((dummych!=33)&&(dummych!=45)&&(dummych!=125)&&
			     (dummych!=126)&&(dummych!=255)&&((dummych<35)||(dummych>41))&&
			     ((dummych<48)||(dummych>57))&&((dummych<64)||
			     (dummych>90))&&((dummych<94)||(dummych>123)))
			  {     
			     eingbeep();
			     break;
			  }
								     /* Zahleneingabetest */
		  case 2: if((eingparam!=3)&&((dummych=='+')||(dummych=='-')))
			  {
			     eingbeep();
			     break;
			  }
		  case 1: if((eingparam!=3)&&((dummych != 'E') && (dummych != 'e') && (dummych != '+') && (dummych != '-') &&
			    (dummych != '.') && (dummych != 32) && ((dummych <48) || (dummych > 57))))
			  {
			     eingbeep();
			     break;
			  }
		  case 0: if (dummych<32) 	                     /* Texteingabe */
			  {
			     eingbeep();
			     break;
			  }
			  if (i<eingstrlaenge)
			  {
			     eingstr[i] = (char) dummych;
			     if(i < einglen - 1)    i = i + 1;
			     ausgabe_eingstr(x_lipos_backgr, y_obpos_backgr, eingstr,
					     backgrlaenge, backgrhoehe, einglen, param);
			  }
			  else                                       /* AnfÅgen an Stringende */
			  {
			     eingstr[i] = (char) dummych;
			     eingstrlaenge = eingstrlaenge + 1;
			     eingstr[eingstrlaenge] = '\x0';
			     if(i < einglen - 1)    i = i + 1;
			     ausgabe_eingstr(x_lipos_backgr, y_obpos_backgr, eingstr,
					     backgrlaenge, backgrhoehe, einglen, param);
			  }
			  break;
		  default: eingbeep();
	       } 						     /* Ende switch: 'Zeichen in String einfuegen' */
      } 							     /* Ende switch: Auswertung Zeicheneingabe     */
   } 								     /* Ende While-Schleife Eingabeende            */
								     /* Ende der While-Schleife mit abschluss >= 0 */
   if(eingparam == 1)    eingdat = strtod(eingstr, &endptr);
   if(eingparam == 2)    eingdat = fabs(strtod(eingstr, &endptr));
   if((eingparam == 1) || (eingparam == 2))
   {
      gcvt(eingdat, einglen, eingstr);
      *einptr=eingdat;
   }
   eingstr[einglen] = '\x0';
   if(eingparam == 0) strcpy((char *) eingwert,eingstr);
   if(eingparam == 3) 
   {
      strupr(eingstr);
      strcpy((char *) eingwert,eingstr);
   }    
   ausgabe_eingstr(x_lipos_backgr, y_obpos_backgr, eingstr,
		   backgrlaenge, backgrhoehe, einglen, param);
   return(abschluss);
}

/***************************************************************************/
/*                               Calerahmen                                */
/*                       Bildschirmrahmen erstellen                        */
/*                                                                         */
/*              (C)  ELAC Technische Software, Kiel  1994		   */
/*                       Ver. 2.0          26.03.94			   */
/*                                                                         */
/*                                                                         */
/* öbergabetextparameter:                                                  */
/* rahtextueber[]  =  Header Text (<= 75 Characters)                       */
/*                                                                         */
/* Benîtigte Globale Variable                                              */
/* gmode           =  Graphic mode: 1 = 640x350,2 = 640x480                */
/*                                                                         */
/* Funktion benîtigt die Datei CALE.CHR                                    */
/*                                                                         */
/* Der Aufruf der Funktion erfolgt Åber die globale Funktion 'Calerahmen', */
/* die lokale Funktion 'Calerahmenbox' mu· nicht deklariert werden.        */
/* Die Funktion 'Calerahmen' lîscht nicht den Bildschirm und setzt nicht   */
/* die Bildschirmhintergrundfarbe, dies mu· das aufrufende Programm durch- */
/* fÅhren !                                                                */
/*                                                                         */
/***************************************************************************/



#define RAHFARB          WHITE                                       //Frame Color
#define RAHTEXTFARB      DARKGRAY                                    //Text Color
#define RAHBOXHAUPTFARB  WHITE                                       //Window Color (Text Background) 
#define RAHBOXOBENFARB   LIGHTGRAY                                   //Color of top side of Window
#define RAHBOXRANDFARB   DARKGRAY                                    //Color of Edge of Window 
#define RAHBOXSEITFARB   DARKGRAY                                    //Color of right side of Window
#define DELTAX           4          





/* lokale Funktion Calerahmenbox */

void Calerahmenbox(int ypos,int hoehe,int deltay,int abstand,char *text)
{
  int xpos,breite,polypoints[20];

  breite= textwidth(text)+16;
  xpos = (640 - breite + DELTAX)>>1;
								     /* Textbox, right side */
  polypoints[0] = xpos + breite;			             
  polypoints[1] = ypos + deltay;
  polypoints[2] = xpos + breite + DELTAX;
  polypoints[3] = ypos;
  polypoints[4] = xpos + breite + DELTAX;
  polypoints[5] = ypos + hoehe;
  polypoints[6] = xpos + breite;
  polypoints[7] = ypos + hoehe + deltay;
  polypoints[8] = xpos + breite;
  polypoints[9] = ypos + deltay;

  setcolor(RAHBOXOBENFARB);
  setfillstyle(1, RAHBOXSEITFARB);
  fillpoly(5, polypoints);
								     /* Textbox front side */ 
  setcolor(RAHBOXRANDFARB);
  setfillstyle(1, RAHBOXHAUPTFARB);
  bar3d(xpos,ypos+deltay,xpos+breite,ypos+deltay+hoehe,1,0);
								     /* Textbox, upper side */	  
  polypoints[0] = xpos;				             
  polypoints[1] = ypos + deltay;
  polypoints[2] = xpos + breite;
  polypoints[3] = ypos + deltay;
  polypoints[4] = xpos + breite + DELTAX;
  polypoints[5] = ypos;
  polypoints[6] = xpos + DELTAX;
  polypoints[7] = ypos;
  polypoints[8] = xpos;
  polypoints[9] = ypos + deltay;

  setfillstyle(1, RAHBOXOBENFARB);
  fillpoly(5, polypoints);
								     /* VerlÑngerung der Rahmenlinie */
  setcolor(RAHFARB);
  line(xpos + breite + (DELTAX>>1), abstand,xpos + breite + DELTAX, abstand);
								     /* Display Headline Text */
  setcolor(RAHTEXTFARB);
  outtextxy(xpos+((DELTAX+breite)>>1),ypos+(hoehe>>1)+(deltay>>1),text);
}





/* globale Funktion Calerahmen als Einsprungspunkt */

void Calerahm(char *rahtextueber)
{
  int fonthoehe,boxhoehe,deltay,rahmenabstand,verschieb,xverschieb,user;
  float faktx;

  if (strlen(rahtextueber)>75) rahtextueber[75] = '\x0';             /* Header text, max 75 Characters long */

  if(gmode == 2)
  { 
     fonthoehe =  16;
     deltay    =   4;
     faktx     = 1.0;
  }
  else
  { 
     fonthoehe = 12;
     deltay    =  3;
     faktx     =  1.37143;
  }

  user=installuserfont("CALE");
  settextstyle(user,HORIZ_DIR,0);
  settextjustify(CENTER_TEXT,CENTER_TEXT);
  setusercharsize(8,8,fonthoehe,9);

  boxhoehe = 1.5 * textheight(rahtextueber);
  rahmenabstand = deltay+(boxhoehe>>1);
  verschieb=int(479/faktx);
  xverschieb=int(rahmenabstand*faktx);

  setlinestyle(SOLID_LINE,0,NORM_WIDTH);
  setviewport(21, 0, 621, boxhoehe+deltay, 1);
  clearviewport();						     /* Lîscht obere Textbox Åber die volle Breite */
  setviewport(0, 0, getmaxx(), getmaxy(), 1);

  setcolor(RAHFARB);
  rectangle(xverschieb, rahmenabstand,	                 	     /* Rahmen zeichnen */
	    639 - xverschieb, verschieb - rahmenabstand);

  line(xverschieb - 1, rahmenabstand,xverschieb-1,                   /* Die beiden senkrechten Rahmenlinien werden */
       verschieb - rahmenabstand); 				     /* doppelt breit gezeichnet.                  */

  line(640 - xverschieb, rahmenabstand,640 - xverschieb,
       verschieb - rahmenabstand);
								     /* Headline Text ausgeben */
  Calerahmenbox(0,boxhoehe,deltay,rahmenabstand,rahtextueber);
								     /* Underline Text ausgeben*/
  Calerahmenbox(int(verschieb-boxhoehe-deltay),boxhoehe,deltay,
		verschieb-rahmenabstand,"ELAC Technische Software  Kiel");
}
void Button(int x1, int y1, int x2, int y2, int colorfill, int colorleft)
{
   setcolor(LIGHTGRAY);
   setfillstyle(SOLID_FILL, colorfill);
   bar3d(x1, y1, x2, y2, 0, 0);
   setcolor(colorleft);
   rectangle(x1 + 1, y1 + 1, x2 - 1, y2 - 1);
   rectangle(x1 + 2, y1 + 2, x2 - 2, y2 - 2);
   setcolor(DARKGRAY);
   line(x1 + 3, y2 - 2, x2 - 2, y2 - 2);
   line(x2 - 2, y1 + 3, x2 - 2, y2 - 2);
   setcolor(BLACK);
   line(x1 + 2, y2 - 1, x2 - 1, y2 - 1);
   line(x2 - 1, y1 + 2, x2 - 1, y2 - 1);
}

