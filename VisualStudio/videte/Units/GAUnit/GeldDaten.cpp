// GeldDaten.cpp
//

#include "stdafx.h"

#include "WK_Trace.h"
#include "GeldDaten.h"
/////////////////////////////////////////////////////////////////////////////
// CGeldDaten
CGeldDaten::CGeldDaten()
{
	Clear();
}
/////////////////////////////////////////////////////////////////////////////
CGeldDaten::CGeldDaten(CGeldDaten& source)
{
	Clear();
	strcpy(m_Datum,source.m_Datum);
	strcpy(m_Zeit,source.m_Zeit);
	strcpy(m_Transaktion,source.m_Transaktion);
	strcpy(m_Automat,source.m_Automat);
	strcpy(m_Bankleitzahl,source.m_Bankleitzahl);
	strcpy(m_Kontonummer,source.m_Kontonummer);
	strcpy(m_Betrag,source.m_Betrag);
	m_bCorrected = source.m_bCorrected;
	m_Time = source.m_Time;
}
/////////////////////////////////////////////////////////////////////////////
CGeldDaten::CGeldDaten(LPCSTR lpszDaten)
{
	Set(lpszDaten);
}
/////////////////////////////////////////////////////////////////////////////
void CGeldDaten::Clear()
{
    memset( m_Datum,0, DATUM_LEN+1);
	memset( m_Zeit,0, ZEIT_LEN+1);
	memset( m_Transaktion,0, TA_LEN+1);
	memset( m_Automat,0, GAA_LEN+1);
	memset( m_Bankleitzahl,0, BLZ_LEN+1);
	memset( m_Kontonummer,0, KTONR_LEN+1);
	memset( m_Betrag,0, BETRAG_LEN+1);
	m_bCorrected = 0;
	m_Time = (time_t)0L;
}
/////////////////////////////////////////////////////////////////////////////
void CGeldDaten::Clear(int iWhat)
{
	switch (iWhat)
	{
		case D:	
		    memset( m_Datum, ' ', DATUM_LEN);
			m_bCorrected |= GDC_DATUM;
			break;
		case Z:	
		    memset( m_Zeit, ' ', ZEIT_LEN);
			m_bCorrected |= GDC_ZEIT;
			break;
		case T:	
		    memset( m_Transaktion, ' ', TA_LEN);
			m_bCorrected |= GDC_TA;
			break;
		case A:	
		    memset( m_Automat, ' ', GAA_LEN);
			m_bCorrected |= GDC_GAA;
			break;
		case B:	
		    memset( m_Bankleitzahl, ' ', BLZ_LEN);
			m_bCorrected |= GDC_BLZ;
			break;
		case K:	
		    memset( m_Kontonummer, ' ', KTONR_LEN);
			m_bCorrected |= GDC_KTNR;
			break;
		case W:	
		    memset( m_Betrag, ' ', BETRAG_LEN);
			m_bCorrected |= GDC_BETRAG;
			break;
		default:
			WK_TRACE("Interner Fehler falscher Parameter in CGeldDaten::Clear\n");
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CGeldDaten::CheckDigitOrSpace(int iWhat)
{
	BOOL bRet = TRUE;
	char* pB;
	int	  i,l,d;

	switch (iWhat)
	{
		case D:	
		    pB = &m_Datum[0];
			l = 6;
			break;
		case Z:	
		    pB = &m_Zeit[0];
			l = 4;
			break;
		case T:	
		    pB = &m_Transaktion[0];
			l = 4;
			break;
		case A:	
		    pB = &m_Automat[0];
			l = 6;
			break;
		case B:	
		    pB = &m_Bankleitzahl[0];
			l = 8;
			break;
		case K:	
		    pB = &m_Kontonummer[0];
			l = 10;
			break;
		case W:	
		    pB = &m_Betrag[0];
			l = 4;
			break;
		default:
			WK_TRACE("Interner Fehler falscher Parameter in CGeldDaten::CheckDigitOrSpace\n");
		    pB = NULL;
			l = 0;
		    bRet = FALSE;
	}

	if (bRet) {
		CString s(pB,l); 
		s.TrimLeft();
		s.TrimRight();
		CString l0('0',l-s.GetLength());
		s = l0 + s;
		d = 0;

		for (i=0;i<l;i++)
		{
			pB[i] = s.GetAt(i);
			if (pB[i]==' ')
			{
				pB[i] = '0';
			}

			if (isdigit(pB[i]))
			{
				d++;
			}
			
		}

		bRet = (d==l);
	}
	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
void CGeldDaten::Validate()
{
	// Datum
	if (!CheckDigitOrSpace(D))
	{
		Clear(D);
	}
	else
	{
		// weitere Verifizierung des Datums
		int	y,m,d;
		char szBuf[3];
		CTime t = CTime::GetCurrentTime();

		szBuf[2] = '\0';

		szBuf[0] = m_Datum[4];
		szBuf[1] = m_Datum[5];
		y = atoi(szBuf);
		szBuf[0] = m_Datum[2];
		szBuf[1] = m_Datum[3];
		m = atoi(szBuf);
		szBuf[0] = m_Datum[0];
		szBuf[1] = m_Datum[1];
		d = atoi(szBuf);

		// Jahr
		if (y != t.GetYear() % 100)
		{
			Clear(D);
		}
		else
		{
			// Monat
			if ((m<1) || (m>12))
			{
				Clear(D);
			}
			else
			{
				int x = 31;
				// Tag
				if (m>7)
					x = (m&1) ? 30 : 31;
				else if (m==2)
					x = (y%4==0) ? 29 : 28;
				else
					x = (m&1) ? 31 : 30;
				
				if ((d<0) || (d>x))
					Clear(D);
			}
		}
	}

	// Zeit
	if (!CheckDigitOrSpace(Z))
	{
		Clear(Z);
	}
	else
	{
		// weitere Verifizierung der Zeit
		int	h,m;
		char szBuf[3];

		szBuf[2] = '\0';

		szBuf[0] = m_Zeit[0];
		szBuf[1] = m_Zeit[1];
		h = atoi(szBuf);
		szBuf[0] = m_Zeit[2];
		szBuf[1] = m_Zeit[3];
		m = atoi(szBuf);

		// Stunde
		if ((h<0) || (h>23))
		{
			Clear(Z);
		}
		else
		{
			if ((m<0) || (m>59))
			{
				Clear(Z);
			}
		}
	}

	// Transaktionsnummer
	if (!CheckDigitOrSpace(T))
	{
		Clear(T);
	}

	// Automatennummer
	if (!CheckDigitOrSpace(A))
	{
		Clear(A);
	}

	// Bankleitzahl
	if (!CheckDigitOrSpace(B))
	{
		Clear(B);			
	}

	// Kontonummer
	if (!CheckDigitOrSpace(K))
	{
		Clear(K);
	}

	// Betrag
	if (!CheckDigitOrSpace(W))
	{
		Clear(W);
	}
}
/////////////////////////////////////////////////////////////////////////////
CString CGeldDaten::Format()
{
	CString sFormat;

	sFormat  = m_Datum[4];
	sFormat += m_Datum[5];
	sFormat += m_Datum[2];
	sFormat += m_Datum[3];
	sFormat += m_Datum[0];
	sFormat += m_Datum[1];

	sFormat += m_Zeit;
	sFormat += m_Transaktion;
	sFormat += m_Automat;
	sFormat += m_Bankleitzahl;
	sFormat += m_Kontonummer;
	sFormat += m_Betrag;
	
	return sFormat;
}
/////////////////////////////////////////////////////////////////////////////
CString CGeldDaten::FormatAusgabeString()
{
	CString sFormat;

	sFormat.Format(	"Zeit: %s TA: %s GAA: %s BLZ: %s Kto Nr: %s Betrag: %s", 
					m_Zeit,
					m_Transaktion,
					m_Automat,
					m_Bankleitzahl,
					m_Kontonummer,
					m_Betrag);
	
	return sFormat;
}
/////////////////////////////////////////////////////////////////////////////
void CGeldDaten::Set(LPCSTR lpszDaten)
{
	Clear();
	if (lpszDaten && lpszDaten[0]){
		strncpy(m_Datum,lpszDaten,DATUM_LEN);
		lpszDaten += DATUM_LEN;
		strncpy(m_Zeit,lpszDaten,ZEIT_LEN);
		lpszDaten += ZEIT_LEN;
		strncpy(m_Transaktion,lpszDaten,TA_LEN);
		lpszDaten += TA_LEN;
		strncpy(m_Automat,lpszDaten,GAA_LEN);
		lpszDaten += GAA_LEN;
		strncpy(m_Bankleitzahl,lpszDaten,BLZ_LEN);
		lpszDaten += BLZ_LEN;
		strncpy(m_Kontonummer,lpszDaten,KTONR_LEN);
		lpszDaten += KTONR_LEN;
		strncpy(m_Betrag,lpszDaten,BETRAG_LEN);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CGeldDaten::Trace()
{
	CString sTrace;

	sTrace.Format("Datum:<%s>,Zeit:<%s>,TAN:<%s>,AN:<%s>,BLZ:<%s>,Konto:<%s>,Betrag:<%s> ",
		      m_Datum,m_Zeit,m_Transaktion,m_Automat,m_Bankleitzahl,m_Kontonummer,m_Betrag);
	
	if (m_bCorrected & GDC_DATUM)
	{
		sTrace += "Datum ";
	}
	if (m_bCorrected & GDC_ZEIT)
	{
		sTrace += "Zeit ";
	}
	if (m_bCorrected & GDC_TA)
	{
		sTrace += "TAN ";
	}
	if (m_bCorrected & GDC_GAA)
	{
		sTrace += "AN ";
	}
	if (m_bCorrected & GDC_BLZ)
	{
		sTrace += "BLZ ";
	}
	if (m_bCorrected & GDC_KTNR)
	{
		sTrace += "KTNR ";
	}
	if (m_bCorrected & GDC_BETRAG)
	{
		sTrace += "Betrag ";
	}
	if (m_bCorrected !=0)
	{
		sTrace += "wurde(n) korrigiert";
	}
	sTrace += '\n';

	WK_TRACE((const char*)sTrace);
}
