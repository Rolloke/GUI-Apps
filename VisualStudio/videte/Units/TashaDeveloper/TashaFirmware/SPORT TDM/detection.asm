/********************************************************************/
/* Differenzbild und Bewegungserkennung								*/
/********************************************************************/
/* Autor: Christian Lohmeier										*/
/* Datum: 01.09.2003												*/
/* Firma: Videte IT	AG												*/
/********************************************************************/
#include <asm_sprt.h>
#include <defBF533.h>
#include "definitionen.h"

/*------------------------------------------------------------------*/
/* Datensektion A in L1				     							*/		
/*------------------------------------------------------------------*/
.section L1_data_a;
.byte _byMask_B[0x654] = "mask.dat";	// File mask.dat laden und Maske miteinbinden
/*------------------------------------------------------------------*/
/* Programmsektion in L1				     						*/		
/*------------------------------------------------------------------*/
.section L1_code;

// Globale Variablen		// Funktionendeklaration
.global _diff;				// erzeugt Differenzbildzeiger auf Speicher, muss übergeben werden
.global _max_search;		// Sucht das Maximum im Differenzbild und speichert Koordinaten
.global _adaptiv_mask;		// erzeugt adaptive Maske
.global _clear1;			// löscht das erste Koordinatenpaar
.global _clear2;			// löscht die 4 weiteren Koordinatenpaare
.global _framec;			// schreibt den Framezählerstand in den Header
.global _clk;				// setzt die Taktrate des Prozessors auf 540 MHz

// Evterne Variablen
.extern _byMax;				// Maximalwert des Differenzbildes
.extern _byFrameBuff_A;		// Zwischenspeicher A
.extern _byFrameBuff_B;		// Zwischenspeicher B
.extern _byFrameBuff_C;		// Differenzbildspeicher	
.extern _bLed;				// Alarmmelder
.extern _byHeader;			// Header Anhang für zusätzliche Bildinformationen
.extern _byMask_A;			// adaptive Maske 

//--------------------------------------------------------------------------//
// Funktion: _diff						 									//
// Beschreibung: Erzeugt aus 8 * 8 Blöcken Differenzwerte	 				//
// 	              und speichert sie in byFrameBuff_C 	 					//
//				 		               										//	
//--------------------------------------------------------------------------//
_diff:
	p2 = r0;				// Adresse von byFrameBuff_C in p2

	i2.l = _byFrameBuff_A;	// Adressen von byFrameBuff_A in Indexregister laden
	i2.h = _byFrameBuff_A;
	i3.l = _byFrameBuff_B;	// Adressen von byFrameBuff_B in Indexregister laden
	i3.h = _byFrameBuff_B;


	m2 = (COL/2)-(BLOCK/2)(z);		//Modifyregister auf 356 Sprung in die nächste Zeile
	m3 = (COL/2)*BLOCK-BLOCK(z);	//Modifyregister auf 2872 Rücksprung 

	p4 = BLOCK(z);					// p4 mit 8 laden für 8 Pixel vertikal
	p5 = (COL/2)/BLOCK(z);			// p5 mit 45 laden für 45 * (8*8) Blöcke horizontal	
	a0 = 0;							// Akkumulator löschen
	a1 = 0;							// Akkumulator löschen
	r1 = 0(z);						// r1, r6, i0 löschen
	r6 = 0(z);
	i0 = 0(z);
	
	lsetup(block_start, block_end) lc1=p5;	// Bildet 45 Differenzwerte im byFrameBuff_C
	block_start:
		lsetup(achtpixel_start, achtpixel_end) lc0=p4; 	// Bildet 1 Differenzwert über 8*8 Pixel
		achtpixel_start:
			r0 = [i2++];		// Lädt 4 Y-Werte aus Buffer A in r0 und erhöht i2
			r2 = [i3++];		// Lädt 4 Y-Werte aus Buffer B in r2 und erhöht i3  
			// Bildet ABS(r0-r2) im Akku und lädt die nächsten 4 Y Werte und erhöht um m2
			saa (r1:0, r3:2) || r0 = [i2++m2] || r2 = [i3++m2];	
		achtpixel_end: saa (r1:0, r3:2) ;       	// Bildet ABS(r0-r2) im Akku 
		r3 = a1.l + a1.h, r4 = a0.l + a0.h;	// Differenzwerte des Akkus in r3 und r4 aufsummieren
		r3 >>= 5;							// Mittelwert bilden
		r4 >>= 5;							// Mittelwert bilden
		r6.l = r3.l + r4.l(ns);	// Mittelwerte zusammenführen und in r6 speichern 
		r6 >>= 1;			// Mittelwert bilden
		b[p2++] = r6;		// r6 im Differenzbild speichern und p2 erhöhen
		i2-=m3;				// Auf den Anfang des nächsten 8*8 Blockes zeigen
		i3-=m3;				// Auf den Anfang des nächsten 8*8 Blockes zeigen

		a0 = 0;				// Akku löschen
	block_end: a1 = 0;		// Akku löschen
_diff.end:  				// eine Zeile im Differenzbild ist fertig
rts; 

_max_search:
	
	r7 = r0;				// Position der Koordinaten im Header übergeben
	p0.h = _byFrameBuff_C;	// Adressen von Differenzwertbuffer byFrameBuff_C  in p0 laden 
	p0.l = _byFrameBuff_C;
	i0.h = _byMax;			// Adressen von byMax Maximalwert in i0 laden
	i0.l = _byMax;
	i1.h = _bLed;			// Adressen von bLed Alarmmelder in i1 laden
	i1.l = _bLed;
	p3.l = _byMask_A;		// Adressen der adaptiven Maske_A  in p3 laden
	p3.h = _byMask_A;	
	p1.h = _byMask_B;		// Adressen der Maske_B  in p1 laden
	p1.l = _byMask_B;
	p5 = (ROW/2)/BLOCK(z);	// Vertikal 36 Zeilen im Differenzbild
	p4 = (COL/2)/BLOCK(z);	// Horizontal 45 Spalten im Differenzbild
	r5 = (COL/2)/BLOCK(z);	// Horizontal 45 Spalten im Differenzbild in r5 speichern
	r6 = (COL/2)/BLOCK(z);	// Horizontal 45 Spalten im Differenzbild in r6 speichern
	r3 = 0(z);				// r3 Löschen
	r4 = 0(z);				// r4 Löschen

	r0 = [i0];				// Hinterlegten Maximalwert in r0 laden
	lsetup(row_start, row_end) lc1=p5;		// Alle 36 Zeilen durchlaufen
	row_start:
		lsetup(col_start, col_end) lc0=p4;	// Alle 45 Spalten durchsuchen
		col_start:
			r1 = b[p0++](z);		// Differenzwert aus FrameBuff_C
			r1 <<= 8;				// Wegen Bewertung mit Maske gedachtes Komma 
			r2 = b[p1](z);			// Wert der Maske_B in r2 schreiben;(wenn alle Werte aus mask.dat (*1) r2 = b[p1++](z);)
			r1.l = r1.l * r2.l(fu);	// Differenzwert mit Maskenwert bewerten
			r1 <<= 8;				// korrigieren wegen der zweiten Maske_A
			r2 = b[p3++](z);		// Wert der Maske_A in r2 schreiben
			r1.l = r1.l * r2.l(fu);	// Differenzwert mit Maskenwert bewerten
			r2 = THRESHOLD(z);  	// Alarmschwelle in r2 laden
			CC = r2 < r1;			// Vergleich: Wenn Wert größer ist als Schwelle CC = 1 
			if CC jump Alarm;		// wenn CC = 1 Sprung zum Alarmhandling
			Alarm_Ready:			// Rücksprungmarke
    		col_end: nop;			// Zeile beendet
	row_end: nop;					// Differenzbild  komplett abgesucht
	
	r2.l = r3.l * r5.l(iu);			// Ort des Maximalwertes berechnen
	r2.l = r2.l + r4.l(ns);			
	r6.l = _byFrameBuff_C;			// Adresse des Differnzbildbuffers byFrameBuff_C in r6
	r6.h = _byFrameBuff_C;
	r6.l = r6.l + r2.l(ns);			// Adresse um Offset zum Maximalwert erhöhen
	p2 = r6;						// Neue Adresse in p2 verschieben			
	nop;					// Stau in der Instructionpipeline vermeiden 
	r5 = 0(z);				// r5 Löschen
	[i0] = r0;				// Maximalwert speichern
	nop;					// Stau in der Instructionpipeline vermeiden
	b[p2] = r5; 			// Maximalwert im Differenzbild löschen 

_max_search.end:
rts;


Alarm:					// Alarmbehandlung
    r2 = 1(z);			// r2 mit 1 laden				
	[i1] = r2;			// Alarmmelder auslösen (bLed = true)
	CC = r0 < r1;		// Wenn Wert größer als vorheriger Maximalwert CC = 1
	if CC jump maximum;	// Wenn CC =1 zum Maximum springen
jump Alarm_Ready;		// Rücksprung bei Beendigung der Alarmabarbeitung

maximum:				// Maximalwert registrieren
	p2.h = _byHeader;	// Adresse des Headers in p2
	p2.l = _byHeader;
	r0 = r1;			// Alten Maximalwert mit dem Neuen überschreiben
	r4 = lc0;			// Zählerstand der Spalten in x Richtung in r4
	r5 = (COL/2)/BLOCK(z);	// r5 = 45
	r6 = (ROW/2)/BLOCK(z);	// r6 = 36
	r4.l = r5.l - r4.l(ns);	// X-Koordinate des Maximalwertes berechnen
	p5 = r7;				// Offset der Koordinaten im Header in p5 übergeben
	nop;					// Stau in der Instructionpipeline vermeiden
	r3 = lc1;				// Zählerstand der Zeilen in Y-Richtung in r3
	r3.l = r6.l - r3.l(ns);	// Y-Koordinate des Maximalwertes berechnen
	nop;					// Stau in der Instructionpipeline vermeiden
	p2 = p2 + p5;			// Schreibposition im Header bestimmen
	b[p2++] = r4; 			// X-Koordinate schreiben
	b[p2] = r3;				// Y-Koordinate schreiben
	p5 = (ROW/2)/BLOCK(z);	// p5 erneut mit 36 laden
jump Alarm_Ready;			// Rücksprung bei Beendigung der Maiximalwertabarbeitung

_adaptiv_mask:
	
	p0.h = _byFrameBuff_C;	// Adressen von Differenzwertbuffer byFrameBuff_C  in p0 laden
	p0.l = _byFrameBuff_C;
	p3.l = _byMask_A;		// Adressen der adaptiven Maske_A  in p3 laden
	p3.h = _byMask_A;
	p5 = (ROW/2)/BLOCK(z);	// Vertikal 36 Zeilen im Differenzbild
	p4 = (COL/2)/BLOCK(z);	// Horizontal 45 Spalten im Differenzbild

	lsetup(row_start1, row_end1) lc1=p5;		// Alle 36 Zeilen durchlaufen
	row_start1:
		lsetup(col_start1, col_end1) lc0=p4;  	// Alle 45 Spalten durchsuchen
		col_start1:			
			r1 = b[p0++](z);		// Differenzwert aus FrameBuff_C holen und p0 erhöhen 
			r1 <<= 8;				// Kommakorrektur 
			r2 = b[p3](z);			// Wert aus der adaptiven Maske holen 
			r2 <<= 1;				// Mit 2 multiplizieren zur Aufweitung der Maske
			r1.l = r1.l * r2.l(fu);	// Differenzwert mit Maske_A bewerten
			r2 = THRESHOLD1(z);		// Schaltschwelle in r2 
			CC = r2 < r1;			// Wenn Wert größer als Schwelle CC = 1
			if CC jump mask_add;	// Wenn CC = 1 Sprung zum Maske_A erhöhen
			mask_ready:     		// Rücksprungmarke
			r2 = b[p3](z);			// Maskenwert einlesen
    		r6 = 0xFF(z);			// Maximalwert in r6
			CC = r2 == r6;			// Wenn Maskenwert = Maximalwert; CC = 1
    		if CC jump adaptiv_end;	// Wenn CC = 1 Sprung zum Ende
			r2 += 1;			// Maskenwert um 1 erhöhen
    		b[p3] = r2;			// und in Mask_A speichern
		   	adaptiv_end:		// Sprungmarke Ende
		  	p3 +=1;				// Zeiger auf den nächsten Maskenwert
    		col_end1: nop;		// Ende der Zeile
	row_end1: nop;				// Ende aller Zeilen

_adaptiv_mask.end:
rts;					// Zurück zum aufrufenden Programm

mask_add:				// Maske erhöhen bedeutet ein Verringern des Maskenwertes
    r2 = b[p3](z);		// Maskenwert holen und in r2 speichern 
    r2 += -MOD;			// Maskenwert um MOD verringern
   	r6 = 0x00(z);		// Minimalwert laden
	CC = r2 < r6;		// Wenn Maskenwert kleiner als Minimalwert; CC = 1
    if CC jump adaptiv_zu;	// Wenn CC = 1 Sprung zum adaptiv_zu:
adaptiv_clear:    				
   	 b[p3] = r2;  		// Wert in Maske_A übernehmen
jump mask_ready;		// Rücksprung 

adaptiv_zu:
	r2 = 0x00(z);		// Kleinsten Wert in r2 laden
jump adaptiv_clear;		// Rücksprung zu adaptive_clear

	
_framec:				// Schreibt den Framecounter in den Header
	p1.l = _byHeader;	// Adresse des Headers in p1 übernehmen
	p1.h = _byHeader;

	w[p1+(OFF+10)] = r0;// Framecounter Zählstand in Header übernehmen
_framec.end:
rts;					// Zurück zum aufrufenden Programm

_clear1:				// Löscht das erste Koordinatenpaar 
p1.l = _byHeader;		// Adresse des Headers in p1 übernehmen	
	p1.h = _byHeader;
	r0.l = 0xFFFF;		// Definierten Wert in r0 schreiben
	r1.l = 0;
	w[p1+OFF] = r0;		// Koordinatenpaar überschreiben
_clear1.end:
rts;					// Zurück zum aufrufenden Programm

_clear2:					// Löscht die 5 Koordinatenpaare
	p1.l = _byHeader+OFF;	// Adresse des Headers plus Offset in p1 übernehmen
	p1.h = _byHeader+OFF;
	r0.l = 0xFFFF;			// Definierten Wert in r0 schreiben
	r1.l = 0;
	p0 = 5;					// Alle fünf Koordinatenpaare löschen
	lsetup(cross_start, cross_end) lc0=p0; 
	cross_start:
		w[p1++] = r0;
	cross_end: nop;
_clear2.end:
rts;					// Zurück zum aufrufenden Programm

_clk:

	p0.h = hi(PLL_CTL);	// Laden der oberen 16 Bit des PPL Registers
	p0.l = lo(PLL_CTL);	// Laden der unteren 16 Bit des PPL Registers

	CLI R2; 			// Interrupts aus, IMASK in r2 kopieren
	R1.L=0x2800; 		// VCO Faktor auf 21x 27MHz
	W[P0]=R1;			//  durch schreiben in PLL_CTL Register
	IDLE; 				// Wartet bis die Pipeline frei ist und PPL Wake Up eintrifft
	STI R2;				// Stellt die alte IMASK wieder her und Interrupts werden enabled
_clk.end:
rts;					// Zurück zum aufrufenden Programm

