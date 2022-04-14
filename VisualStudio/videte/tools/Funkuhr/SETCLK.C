/* File : SETCLK.C   Translated 10/91 */
/* using P2C Version 2.06 TP by LAUER & WALLWITZ D-6200 Wiesbaden */

#define  P2CLIB '4'
#include <dos.h>
#include <p2c.h>
#include <setclk.h>

static BYTE far count_1(BYTE bits);

DATUMZEIT dcfdata;
SHORT hilf,faktor;
BYTE nr,inbit;
BOOL atom_zeit,active,count,sommerzeit,zeit_ok,datum_ok,mesz;
BIT59 time1,time2;
USHORT time_diff0,time_diff1,time_diff1a,time_diff2,time_diff3;
BYTE zaehler;
USHORT origseg,origofs;
T_COMADRESSES (*comadresses)=MK_FP(0,0x0400)    /* BIOS COMx adresses */
;

VOID far initv24(SERIAL com,LONG baud,BYTE bits,BYTE stop,PARITY par)
{
  USHORT c;
  BYTE dummyb;
  
  c=((*comadresses)[(USHORT)(com)+1-1] & A_MSK);/* get high com port adresses */
  outportb(c+COMLC,0x00);       /* disable baud rate access */
  outportb(c+COMIE,0x00);       /* disable interrupts */
  outportb(c+COMLC,0x80);       /* enable baud rate access */
  outportb(c+COMDLL,(BYTE)((USHORT)(115200/baud)));
  outportb(c+COMDLH,(BYTE)(((USHORT)(115200/baud))>>8));
  outportb(c+COMLC,(bits-5)+(shl((stop-1),2))+(shl((USHORT)(par),3)));
  dummyb=inportb(c+COMRX);
}



VOID far rts(SERIAL com,BOOL x) /* force Ready to Send Output */
{
  if (x) outportb((((*comadresses)[(USHORT)(com)+1-1] & A_MSK))+COMMC,(inportb
    ((((*comadresses)[(USHORT)(com)+1-1] & A_MSK))+COMMC) | 2));
  else outportb((((*comadresses)[(USHORT)(com)+1-1] & A_MSK))+COMMC,(inportb
    ((((*comadresses)[(USHORT)(com)+1-1] & A_MSK))+COMMC) & (~2)));
}



VOID far dtr(SERIAL com,BOOL x) /* force Data Terminal Ready Output */
{
  if (x) outportb((((*comadresses)[(USHORT)(com)+1-1] & A_MSK))+COMMC,(inportb
    ((((*comadresses)[(USHORT)(com)+1-1] & A_MSK))+COMMC) | 1));
  else outportb((((*comadresses)[(USHORT)(com)+1-1] & A_MSK))+COMMC,(inportb
    ((((*comadresses)[(USHORT)(com)+1-1] & A_MSK))+COMMC) & (~1)));
}



CHAR far rcom(SERIAL com)       /* Receive character */
{
  CHAR r_rcom;
  
  while ((inportb((((*comadresses)[(USHORT)(com)+1-1] & A_MSK))+COMLS) & COMRREADY
  )==0) ;
  r_rcom=(CHAR)(inportb((((*comadresses)[(USHORT)(com)+1-1] & A_MSK))+COMRX
  ));
  return r_rcom;
}



BOOL far test_rcom(SERIAL com)  /* Test Receiver for character */
{
  BOOL r_test_rcom;
  
  r_test_rcom=!((inportb((((*comadresses)[(USHORT)(com)+1-1] & A_MSK))+COMLS
  ) & COMRREADY)==0);
  return r_test_rcom;
}



VOID far init_time(BIT59 bits)
{
  bits[0]=0;    /* Alle bits lîschen */
  bits[1]=0;
  bits[2]=0;
  bits[3]=0;
}



VOID far test_empfaenger(VOID)
/* PrÅfe ob EmpfÑnger Daten sendet */
{
  BYTE sek,tic;
  struct time zeit;
  tic=0;
  do {
    gettime(&zeit);
    sek=zeit.ti_sec;
    if (tic==0) tic=(sek+WAIT_SEC)%60;
    count=test_rcom((SERIAL)(nr-1));    /* EmpfÑnger vorhanden */
  } while (!((count) || (sek==tic)));   /* nach 3 Sekunden Ende */

  if (!count) { /* Wenn kein Signal empfangen wurde */
    printf("\x07%s%d\n",EMPFAENGER_NICHT_DA,nr);/* Fehlermeldung ausgeben */
    exit(1);
  }
}



VOID far init_empfaenger(VOID)
{
  zeit_ok=FALSE;/* Flags lîschen */
  datum_ok=FALSE;
  init_time(time1);
  init_time(time2);     /* time1 und time2 auf 0 setzen */
  /* Es folgt die Initialisierung von Port COM(nr) mit 50 Baud */
  initv24((SERIAL)(nr-1),50,8,1,NO);
  dtr((SERIAL)(nr-1),TRUE);     /* Serielle Schnittstelle */
  rts((SERIAL)(nr-1),FALSE);    /* fÅr Empfang vorbereiten */
}



VOID far shr59(BIT59 bits,BYTE inbit)   /* Shifted 59 Bits 1 Bit nach rechts */
{
  BYTE carry0,carry1,carry2,carry3;
  
  carry0=(bits[0] & 1); /* öbertrÑge vor shiften sichern */
  carry1=(bits[1] & 1);
  carry2=(bits[2] & 1);
  carry3=(bits[3] & 1);
  bits[0]=shr(bits[0],1);       /* Alle Variablen des Arrays*/
  bits[1]=shr(bits[1],1);       /* um ein Bit nach rechts verschieben */
  bits[2]=shr(bits[2],1);
  bits[3]=shr(bits[3],1);
  if (carry1==1) bits[0]=(bits[0] | 0x8000);    /* öbertrÑge einfÅgen */
  if (carry2==1) bits[1]=(bits[1] | 0x8000);
  if (carry3==1) bits[2]=(bits[2] | 0x8000);
  if (carry0==1) bits[3]=(bits[3] | 0x0400);
  if (inbit!=0) bits[2]=(bits[2] | 0x40);
  else bits[2]=(bits[2] & (~0x40));
}

static BIT59 *l_time;

BOOL far parity_check(BIT59 v_time)     /* prÅft Parity-Bits im Datensignal von DCF77 */
{
  BOOL r_parity_check;
  BIT59 time;
  
  memmove(time,v_time,sizeof(BIT59));
  l_time=(BIT59*)time;
  l_time=(BIT59*)time;
  r_parity_check=FALSE;
  if (count_1(20)) ;            /* Diese Bits werden nicht berÅcksichtigt */
  if (((count_1(8) & 1))==1)
    return r_parity_check;      /* 1.Parity-Bit prÅfen */
  if (((count_1(7) & 1))==1) 
    return r_parity_check;      /* 2.Parity-Bit prÅfen */
  if (((count_1(23) & 1))==1) 
    return r_parity_check;      /* 3.Parity-Bit prÅfen */
  r_parity_check=TRUE;
  return r_parity_check;
}



static BYTE far count_1(BYTE bits)
{
  BYTE r_count,t,y;
  
  y=0;
  for (t=1;t<=bits;t++) {
    if ((((*l_time)[2] & 0x80))==0x80)
      (y)++;    /* y erhîhen, wenn Bit gesetzt */
    shr59(*l_time,0);
  }
  r_count=y;
  return r_count;
}



VOID far make_time(BIT59 v_time,DATUMZEIT *dcfdata)     /* Zeit und Datum dekodieren und Stundenverschiebung berechnen */
{
  BIT59 time;
  
  memmove(time,v_time,sizeof(BIT59));
  zeit_ok=FALSE;/* Flag zur Einstellung des Datums lîschen, */
  datum_ok=FALSE;       /* um einen verfrÅhten Aufruf von SetDate und */
  /* einen erneuten Aufruf von make_time zu verhindern. */
  { dcfdata->sec100=10;
    dcfdata->sec=0;     /* Umwandlung der Daten von BCD */
    dcfdata->min=((time[0] & 0x0F))+(((shr(time[0],4)) & 0x07))*10;
    dcfdata->std=(((shr(time[0],8)) & 0x0F))+(((shr(time[0],12)) & 0x03))*10
    ;
    dcfdata->tag=(((shl(time[1],1)) & 0x0F))+(((shr(time[1],3)) & 0x03))*10
    ;
    if (((time[0] & 0x8000))==0x8000) 
      (dcfdata->tag)++;
    dcfdata->mon=(((shr(time[1],8)) & 0x0F))+(((shr(time[1],12)) & 0x01))*10
    ;
    dcfdata->jahr=(((shr(time[1],13)) & 0x07))+((time[2] & 0x01))*8+(((shr(
    time[2],1)) & 0x0F))*10;
    hilf=dcfdata->std+faktor;   /* Da faktor negativ sein kann, mu· eine Integer Variable verwendet werden */
    if (dcfdata->jahr>50) dcfdata->jahr=dcfdata->jahr+1900;
    else dcfdata->jahr=dcfdata->jahr+2000;      /* PrÅfe auf Sommer/Winterzeit-Umstellung,
     um PC-Uhr sekundengenau nachzustellen */
    if (time1[3] & 0x80)
      mesz=TRUE /* Bit fÅr Sommerzeit gesetzt oder Wechsel SZ -> WZ angekÅndigt? */
      ;
    else mesz=FALSE;
    if (mesz) 
      if (!sommerzeit) 
        (hilf)--;       /* Falls keine Umstellung der Zeit */
        /* auf Sommerzeit erfolgen soll */
    dcfdata->std=abs(hilf);     /* Es folgt eine öberprÅfung, ob das Datum aufgrund des Zeitverschiebungsfaktors  */
    /* geÑndert werden mu· */
    if (hilf>23)/* Uhr einen Tag vorstellen */
    { dcfdata->std=hilf-24;
      (dcfdata->tag)++;
      if (dcfdata->tag>28) 
        switch (dcfdata->mon) {
          case 1:
          case 3:
          case 5:
          case 7:
          case 8:
          case 10:
            if (dcfdata->tag>31) {
              (dcfdata->mon)++;
              dcfdata->tag=1;
            }
            break;
          case 4:
          case 6:
          case 9:
          case 11:
            if (dcfdata->tag>30) {
              (dcfdata->mon)++;
              dcfdata->tag=1;
            }
            break;
          case 2:
            if ((dcfdata->jahr%4)==0)   /* Bei Schaltjahr */
            { if (dcfdata->tag>29) {
                (dcfdata->mon)++;
                dcfdata->tag=1;
              }
            } else {
              (dcfdata->mon)++;
              dcfdata->tag=1;
            }
            break;
          case 12:
            if (dcfdata->tag>31) {
              dcfdata->mon=1;
              dcfdata->tag=1;
              (dcfdata->jahr)++;
            }
            break;
        }       /* case */
    }
    if (hilf<0) /* Uhr einen Tag nach stellen */
    { dcfdata->std=hilf+24;
      (dcfdata->tag)--;
      if (dcfdata->tag<1) {
        (dcfdata->mon)--;
        switch (dcfdata->mon) {
          case 1:
          case 3:
          case 5:
          case 7:
          case 8:
          case 10:
            dcfdata->tag=31;
            break;
          case 4:
          case 6:
          case 9:
          case 11:
            dcfdata->tag=30;
            break;
          case 2:
            if ((dcfdata->jahr%4)==0) dcfdata->tag=29   /* Bei Schaltjahr */
              ;
            else dcfdata->tag=28;
            break;
          case 0:
            dcfdata->mon=12;
            dcfdata->tag=31;
            (dcfdata->jahr)--;
            break;
        }       /* of case */
      }
    }
  }/* of with */
  datum_ok=TRUE;/* Datum darf neu gesetzt werden */
  zeit_ok=TRUE; /* Einlesen der neuen Daten wieder erlaubt */
}



VOID far readdcf(VOID)
{
  BYTE hilf_min,hilf_sec;
  SHORT diff;
  
  if ((!zeit_ok))       /* öberprÅfung, ob die Daten bereits komplett
                          eingelesen wurden */
    if (test_rcom((SERIAL)(nr-1)))      /* öberprÅfung, ob ein Signal am seriellen Port */
    /* eingelesen werden mu· */
    { inbit=(USHORT)(rcom((SERIAL)(nr-1)));     /* Einlesen des Signals */
      putch((CHAR)(inbit));
      if ((inbit==0)||(inbit==128)||(inbit==192)||(inbit==224))
	inbit=1 /* Auswertung des Signals */
        ;
      else inbit=0;
      if (((time1[2] & 0x80))!=0) shr59(time2,1)/* time1 wird in time2 geschoben */
        ;
      else shr59(time2,0);
      shr59(time1,inbit);       /* inbit in das 6.Bit von time1[2] einfÅgen */
      /* Differenz von neuer und alter Zeit bilden, wobei die PrÅfbits */
      /* und der Wochentag unberÅcksichtigt bleiben */
      time_diff0=((time1[0] & 0xBF7F))-((time2[0] & 0xBF7F));
      time_diff1=((time1[1] & 0xFF1F))-((time2[1] & 0xFF1F));
      time_diff1a=((time1[1] & 0xFF00))-((time2[1] & 0xFF00));
      time_diff2=((time1[2] & 0xFFDF))-((time2[2] & 0xFFDF));
      time_diff3=((time1[3] & 0x0FFF))-((time2[3] & 0x0FFF));   /* PrÅfen, ob zwei Minuten richtig eingelesen wurden */
      if ((((time1[3] & 0x041F))==0x0400) && (((time1[2] & 0xFFC0))==0) && 
      (((time2[3] & 0x041F))==0x0400) && (((time2[2] & 0xFFC0))==0))    /* Nullenfeld und Startbit gefunden */
        if (((time_diff1==0) && (time_diff2==0) &&      /* Obere words gleich */
        (((time_diff0==1) && ((time1[0] & 0x0F)<10))    /* MinutenÑnderung um 1 */
         || ((time_diff0==0x10-9) && ((time1[0] & 0x0F)==0))    /* MinutenÑnderung um 10 */
         || ((time_diff0==0x0100-0x59) && ((time1[0] & 0x7F)==0))       /* StundenÑnderung um 1 und Minuten = 0 */
         || ((time_diff0==0x1000-0x0959) && ((time1[0] & 0x0F7F)==0))   /* StundenÑnderung um 10 und Minuten = 0 */
         || ((time_diff0==0x8000-0x2359) && ((time1[0] & 0x3F7F)==0))   /* TagesÑnderung um 1, Std = Min = 0 */
        )) || ((time_diff2==0) && (((time_diff0==(USHORT)(0-0xA359)) && (time_diff1
        ==8-4) && ((time1[0] & 0xBF7F)==0) && ((time1[1] & 0x07)==0))   /* TagesÑnderung um 10, Std = Min = 0 */
         || (((time_diff0==(USHORT)(0-0x2359)) || (time_diff0==(USHORT)(0-0xA359
        ))) && (time_diff1a==0x0100) && ((time1[0] & 0xBF7F)==0x8000) && ((
        time1[1] & 0x1F)==0))   /* MonatsÑnd. um 1, tag = 1, Uhrz. 0:0 */
         || (((time_diff0==(USHORT)(0-0x2359)) || (time_diff0==(USHORT)(0-0xA359
        ))) && (time_diff1a==0x1000-0x0900) && ((time1[0] & 0xBF7F)==0x8000
        ) && ((time1[1] & 0x1F1F)==0x1000))     /* MonatsÑnd. auf 10, Tag = 1, Uhrzeit 0:0 */
        )) || ((time_diff0==(USHORT)(0-0x2359)) && (time_diff1==0x2000-0x1218
        ))      /* Wechsel 31. auf 1. beim Jahreswechsel */
         || ((time_diff1==0) && (time_diff2==0) && (((time_diff0==0x0300-0x0159
        ) && ((time2[3] & 0x05DF)==0x0540) && ((time1[3] & 0x05DF)==0x04C0)
        )       /* Umstellung Winter -> Sommerzeit */
         || ((time_diff0==(USHORT)(0x0200-0x0259)) && ((time2[3] & 0x05DF)==
        0x04C0) && ((time1[3] & 0x05DF)==0x0540))       /* Umstellung Sommer -> Winterzeit */
        ))) 
          if (parity_check(time1))      /* Wenn Parity-Bits OK */
          { zeit_ok=TRUE;       /* Daten wurden komplett eingelesen und kînnen */
            datum_ok=TRUE;      /* ausgewertet werden */
          }
    }
}



VOID far setdatum(USHORT jahr,USHORT mon,USHORT tag)
{
    struct date datum;
    datum.da_year = jahr;
    datum.da_mon = mon;
    datum.da_day = tag;
    setdate(&datum);
}



VOID far settimemitdos(USHORT std,USHORT min,USHORT sec,USHORT sec100)
{
    struct time zeit;
    zeit.ti_hour = std;
    zeit.ti_min = min;
    zeit.ti_sec = sec;
    zeit.ti_hund = sec100;
    settime(&zeit);
}



BOOL far escape(VOID)
/* Liefert TRUE, wenn <Esc> gerdrÅckt wurde */
{
  BOOL r_escape;
  
  r_escape=FALSE;
  if (!kbhit())
    return r_escape;    /* RÅcksprung, falls keine Taste gedrÅckt wurde */
  if (getch()=='\x1B') r_escape=TRUE;/* <Esc> gedrÅckt? */
  return r_escape;
}



BOOL far readtimefromdcf(VOID)
{
  BOOL r_readtimefromdcf;
  
  readdcf();
  if (zeit_ok)  /* Daten komplett eingelesen? */
  { make_time(time1,&dcfdata);  /* Auswerten der Daten */
    settimemitdos(dcfdata.std,dcfdata.min,dcfdata.sec,dcfdata.sec100);
  }
  if (datum_ok) {
    setdatum(dcfdata.jahr,dcfdata.mon,dcfdata.tag);
  }
  if (datum_ok && zeit_ok) r_readtimefromdcf=TRUE;
  else r_readtimefromdcf=FALSE;
  if (escape()) {       /* Wenn <Esc> gedrÅckt wurde */
    putch('\n');
    printf("\x07%s\n",ABBRUCH); /* Abbruch-Meldung ausgeben */
    exit(0);    /* Programm abbrechen */
  }
  return r_readtimefromdcf;
}


SHORT main(SHORT argc,PSZ argv[])
{
  /* Hauptprogramm */
  /* Auswertung der Command-Line-Parameter */
  _argc=--argc;
  _argv=argv;
  hilf=atoi(argv[1]);
  nr=hilf;
  if (argc==0) nr=DEFAULT_COM   /* Wenn keine Parameter Åbergeben wurden */
    ;
  else if ((hilf==0) || (nr<1) || (nr>4)) {
    if (argv[1]=="?") {
      putch('\n');
      printf("%s\n",READ2);
      putch('\n');
      printf("%s\n",READ3);
      printf("%s%s%s%s%c\n",TAB,TAB,TAB,VOREINST,(CHAR)(DEFAULT_COM
      +(USHORT)('0')));
      printf("%s\n",READ4);
      printf("%s%s%s%s%c\n",TAB,TAB,TAB,VOREINST,(CHAR)(DEFAULT_ZEIT
      +(USHORT)('0')));
      printf("%s\n",READ5);
      printf("%s%s%s%s%s\n",TAB,TAB,TAB,VOREINST,AN);
      exit(0);
    } else {
      printf("\x07%s%*.*s\n",ERROR_COM_PORT_UG,sizeof(argv[1]),sizeof
      (argv[1]),argv[1]);
      exit(1);
    }
  }
  if ((*comadresses)[nr-1]==0) {/* Wenn COM(nr) nicht vorhanden ist */
    printf("\x07%s%d\n",ERROR_COM_PORT_NG,nr);
    exit(1);
  }
  if (argc>1)   /* Wenn mehr als ein Parameter Åbergeben wurde */
  { hilf=atoi(argv[2]);
    faktor=hilf;
    if ((hilf==0) || (abs(faktor)>13))  /* Wenn ParamStr(2) nicht in eine Zahl umgewandelt */
    {/* werden kann oder der Wert 13 Åbersteigt */
      printf("\x07%s%*.*s\n",ERROR_ZEIT,sizeof(argv[2]),sizeof(argv
      [2]),argv[2]);
      exit(1);
    }
  } else faktor=DEFAULT_ZEIT;   /* Wenn kein Verschiebungsfaktor angegeben wurde, */
    /* wird faktor auf 0 gesetzt */
  if (argv[3]=="aus") sommerzeit=FALSE  /* Wenn keine MESZ gewÅnscht wird, */
    /* wird das Flag gelîscht, */
    ;
  else sommerzeit=TRUE; /* ansonsten wird es gesetzt */
  init_empfaenger();    /* Initialisierung des EmpfÑngers */
  test_empfaenger();    /* EmpfÑnger vorhanden? */
  active=FALSE;
  atom_zeit=FALSE;      /* Flag fÅr Atomzeit */
  mesz=FALSE;   /* Flag fÅr Sommerzeit */
  zaehler=0;
  putch('\n');
  printf("%s%s%s%d\n",VERS_TEXT,VERS,INSTALL_OK,nr);    /* Installationsmeldung ausgeben */
  while ((!readtimefromdcf())) ;/* Endlosschleife bis PC-Uhr einmal gesetzt */
  putch('\n');
  putch('\n');
  { printf("     Zeit: %2u:%2u:%2u\n",dcfdata.std,dcfdata.min,dcfdata
    .sec);
    printf("    Datum: %2u.%2u.%u\n",dcfdata.tag,dcfdata.mon,dcfdata
    .jahr);
  }
  putch('\n');
  return 0;
}
