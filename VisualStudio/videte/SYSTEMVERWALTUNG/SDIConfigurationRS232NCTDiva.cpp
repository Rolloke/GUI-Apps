// SDIConfigurationRS232NCTDiva.cpp: implementation of the CSDIConfigurationRS232NCTDiva class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "systemverwaltung.h"
#include "SDIConfigurationRS232NCTDiva.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
CSDIConfigurationRS232NCTDiva::CSDIConfigurationRS232NCTDiva(CSDIConfigurationDialog* pParent, 
															 int iCom,
															 BOOL bTraceAscii/*=TRUE*/,
															 BOOL bTraceHex/*=FALSE*/,
															 BOOL bTraceEvents/*=FALSE*/)
	: CSDIConfigurationRS232(pParent, iCom, bTraceAscii, bTraceHex, bTraceEvents)
{
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationRS232NCTDiva::OnReceivedData(LPBYTE pData, DWORD dwLen)
{
/*
<ESC>=<STX>
<VT>  HIER STEHT IHRE         WERBUNG       <ENDE>
<ESC>=<SOH>
<VT>                                        <ENDE>
<VT>Rückgabe            Di 27.05.   EUR 0,00
<VT>Zurückspulen                  EUR   0,85
<VT>Zurückspulen        *2        EUR   1,70
<VT>Gesamt:     0,85 EUR                    <ENDE>
<ESC>p  <ENDE>
*/
	if (m_bReadInData == FALSE) {
		return;
	}

	DWORD dw = 0;
	while (pData && dw<dwLen)
	{
		// <VT> starts a new data record
		if (pData[dw] == ASCII_VT)
		{
			ClearAllData();
		}
		// <ESC> starts a special command
		else if (pData[dw] == ASCII_ESC)
		{
			ClearAllData();
			m_sDataComplete += _T("<ESC>");
			m_iReceivedTotal++;
		}
		// <STX> ends a special command
		else if (pData[dw] == ASCII_STX)
		{
			m_sDataComplete += _T("<STX>");
			m_iReceivedTotal++;
			if (m_sDataComplete == _T("<ESC>=<STX>"))
			{
				m_sDataComplete += _T(" (Startsequenz für Bondrucker)");
				OnDataComplete();
				ClearAllData();
			}
		}
		// <SOH> ends a special command
		else if (pData[dw] == ASCII_SOH)
		{
			m_sDataComplete += _T("<SOH>");
			m_iReceivedTotal++;
			if (m_sDataComplete == _T("<ESC>=<SOH>"))
			{
				m_sDataComplete += _T(" (Endsequenz für Bondrucker)");
				OnDataComplete();
				ClearAllData();
			}
		}
		// "<ESC>p  " command for cash system
		else if (pData[dw] == ' ')
		{
			m_sDataComplete += pData[dw];
			m_iReceivedTotal++;
			if (m_sDataComplete == _T("<ESC>p  "))
			{
				m_sDataComplete += _T(" (Schublade)");
				OnDataComplete();
				ClearAllData();
			}
		}
		else
		{
			m_sDataComplete += pData[dw];
			m_iReceivedTotal++;
		}
		// Wenn alle Zeichen empfangen wurden, ist ein Datensatz komplett
		if (m_iReceivedTotal == 40)
		{
			OnDataComplete();
			ClearAllData();
		}
		dw++;
	} // while
}
